
// #include <glib/gi18n.h>
#include <scroll_button.h>

struct _ScrollButton
{
        GtkWidget parent;
        GtkWidget *child;

        GtkGesture *gesture;
        AdwAnimation *scroll_animation;

        gint max_width;
        gchar *label;
        gdouble default_label_width;

        gboolean should_play_animation;
};

G_DEFINE_TYPE(ScrollButton, scroll_button, GTK_TYPE_WIDGET)

enum
{
        PROP_0,
        PROP_MAX_WIDTH,
        PROP_LABEL,
        LAST_PROP,
};
enum
{
        SIG_CLICKED,
        LAST_SIGNAL,
};

static GParamSpec *properties[LAST_PROP];
static guint signals[LAST_SIGNAL];

static void
scroll_button_measure(GtkWidget *widget,
                      GtkOrientation orientation,
                      gint for_size,
                      gint *minimum,
                      gint *natural,
                      gint *minimum_baseline,
                      gint *natural_baseline)
{
        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_INT);
        g_object_get_property(G_OBJECT(widget), "max-width", &value);
        ScrollButton *self = (ScrollButton *)widget;

        GtkWidget *child = gtk_widget_get_first_child(widget);

        // for (child = gtk_widget_get_first_child(widget);
        //      child != NULL;
        //      child = gtk_widget_get_next_sibling(child))
        // {
        if (gtk_widget_get_visible(child))
        {
                gint child_min = 0;
                gint child_nat = 0;
                gint child_min_baseline = -1;
                gint child_nat_baseline = -1;

                gtk_widget_measure(child, orientation, g_value_get_int(&value),
                                   &child_min, &child_nat,
                                   &child_min_baseline, &child_nat_baseline);

                if (orientation == GTK_ORIENTATION_HORIZONTAL)
                {

                        if (child_min > g_value_get_int(&value))
                        {
                                self->default_label_width = (gdouble)MAX(*minimum, child_min);
                                adw_timed_animation_set_value_from(ADW_TIMED_ANIMATION(self->scroll_animation), -(self->default_label_width - g_value_get_int(&value)) / 2);
                                adw_timed_animation_set_value_to(ADW_TIMED_ANIMATION(self->scroll_animation), (self->default_label_width - g_value_get_int(&value)) / 2);
                                self->should_play_animation = TRUE;
                                *minimum = g_value_get_int(&value); // MAX(*minimum, child_min);
                                *natural = g_value_get_int(&value); // MAX(*natural, child_nat);
                        }
                        else
                        {
                                self->should_play_animation = FALSE;
                                *minimum = MAX(*minimum, child_min);
                                *natural = MAX(*natural, child_nat);
                        }
                }
                else
                {
                        *minimum = MAX(*minimum, child_min);
                        *natural = MAX(*natural, child_nat);
                }

                if (child_min_baseline > -1)
                        *minimum_baseline = MAX(*minimum_baseline, child_min_baseline);
                if (child_nat_baseline > -1)
                        *natural_baseline = MAX(*natural_baseline, child_nat_baseline);
        }
        // }
        g_value_unset(&value);
}

static void
scroll_button_allocate(GtkWidget *widget,
                       gint width,
                       gint height,
                       gint baseline)
{

        GtkWidget *child = gtk_widget_get_first_child(widget);
        ScrollButton *self = (ScrollButton *)widget;

        if (child && gtk_widget_get_visible(child))
        {

                if (adw_animation_get_state(self->scroll_animation) == ADW_ANIMATION_PLAYING)
                {
                        gtk_widget_allocate(child,
                                            width,
                                            height,
                                            baseline,
                                            gsk_transform_translate(NULL, &GRAPHENE_POINT_INIT(
                                                                              adw_animation_get_value(self->scroll_animation),
                                                                              0)));
                        return;
                }
                else if (self->should_play_animation)
                        adw_animation_play(self->scroll_animation);

                gtk_widget_allocate(child, width, height, baseline, NULL);
        }
}

static void
click_pressed_cb(GtkGestureClick *gesture,
                 guint n_press,
                 double x,
                 double y,
                 GtkWidget *self)
{
        if (gtk_widget_get_focus_on_click(self) && !gtk_widget_has_focus(self))
                gtk_widget_grab_focus(self);
}

static void
click_released_cb(GtkGestureClick *gesture,
                  guint n_press,
                  double x,
                  double y,
                  GtkWidget *self)
{
        gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);

        if (gtk_widget_is_sensitive((self)) && gtk_widget_contains((self), x, y))
                g_signal_emit(self, signals[SIG_CLICKED], 0);
}

static void
_on_timed_animation(double value,
                    GtkWidget *self)
{
        gtk_widget_queue_allocate(self);
}

static void
scroll_button_init(ScrollButton *self)
{
        self->gesture = gtk_gesture_click_new();

        self->should_play_animation = FALSE;

        AdwAnimationTarget *target = adw_callback_animation_target_new((AdwAnimationTargetFunc)_on_timed_animation,
                                                                       self,
                                                                       NULL);

        self->scroll_animation = adw_timed_animation_new(GTK_WIDGET(self), 0, 1, 7000, g_object_ref(target));
        adw_timed_animation_set_alternate(ADW_TIMED_ANIMATION(self->scroll_animation), true);
        adw_timed_animation_set_repeat_count(ADW_TIMED_ANIMATION(self->scroll_animation), 0);
        adw_timed_animation_set_easing(ADW_TIMED_ANIMATION(self->scroll_animation), ADW_EASE_IN_OUT_CUBIC);

        gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(self->gesture), GTK_PHASE_CAPTURE);
        gtk_gesture_single_set_touch_only(GTK_GESTURE_SINGLE(self->gesture), false);
        gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(self->gesture));

        g_signal_connect(self->gesture, "pressed",
                         G_CALLBACK(click_pressed_cb),
                         self);

        g_signal_connect(self->gesture, "released",
                         G_CALLBACK(click_released_cb),
                         self);

        gtk_widget_init_template(GTK_WIDGET(self));
}

static void
scroll_button_dispose(GObject *object)
{
        ScrollButton *self = (ScrollButton *)object;

        g_free(self->label);
        g_clear_pointer(&self->child, gtk_widget_unparent);

        G_OBJECT_CLASS(scroll_button_parent_class)->dispose(object);
}

int scroll_button_get_max_width(ScrollButton *self)
{
        return self->max_width;
}
void scroll_button_set_max_width(ScrollButton *self,
                                 gint max_width)
{
        if (self->max_width != max_width)
        {
                self->max_width = max_width;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MAX_WIDTH]);
        }
}

gchar *scroll_button_get_label(ScrollButton *self)
{
        return self->label;
}
void scroll_button_set_label(ScrollButton *self,
                             const gchar *label)
{
        if (g_strcmp0(label, self->label) != 0)
        {
                self->label = g_strdup(label);

                gtk_label_set_label(GTK_LABEL(self->child), self->label);
                gtk_widget_queue_resize(GTK_WIDGET(self));

                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
        }
}

void scroll_button_get_property(GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec)
{
        ScrollButton *self = SCROLL_BUTTON(object);

        switch (prop_id)
        {
        case PROP_MAX_WIDTH:
                g_value_set_int(value, scroll_button_get_max_width(self));
                break;

        case PROP_LABEL:
                g_value_set_string(value, scroll_button_get_label(self));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void scroll_button_set_property(GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
        ScrollButton *self = SCROLL_BUTTON(object);

        switch (prop_id)
        {
        case PROP_MAX_WIDTH:
                scroll_button_set_max_width(self, g_value_get_int(value));
                break;
        case PROP_LABEL:
                scroll_button_set_label(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

static void
scroll_button_class_init(ScrollButtonClass *klass)
{
        G_OBJECT_CLASS(klass)->dispose = scroll_button_dispose;
        GObjectClass *object_class = G_OBJECT_CLASS(klass);
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/gtkbible/scroll_button.ui");
        gtk_widget_class_set_css_name(widget_class, "button");
        widget_class->size_allocate = scroll_button_allocate;
        widget_class->measure = scroll_button_measure;

        gtk_widget_class_bind_template_child(widget_class, ScrollButton, child);

        object_class->get_property = scroll_button_get_property;
        object_class->set_property = scroll_button_set_property;

        properties[PROP_MAX_WIDTH] = g_param_spec_int(
            "max-width", // Name
            "Chapter",   // Nick
            "Chapter",   // Blurb
            0,
            5000,
            200,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_LABEL] = g_param_spec_string(
            "label", // Name
            "Label", // Nick
            "Label", // Blurb
            "",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        g_object_class_install_properties(object_class, LAST_PROP, properties);

        signals[SIG_CLICKED] = g_signal_new(
            "clicked",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_FIRST,
            0, NULL, NULL, NULL,
            G_TYPE_NONE, 0);
}

ScrollButton *
scroll_button_new()
{
        return (ScrollButton *)g_object_new(SCROLL_TYPE_BUTTON, NULL);
}
