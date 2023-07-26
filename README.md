# GtkBible

- Very early state of a bible application (Usage at own risk, there are still memory leaks and bugs/ crashes).
- Wtitten in C with GTK and Libadwaita.

![](https://github.com/robertomorrison0/GtkBible/blob/master/preview_image.png)

## How to compile

Depending on your package manager you may need *-dev packages or similar to obtain the nessecary (header) files

### Dependencies
- json-c
- libcurl
- gtk
- libadwaita


```
meson setup build
cd build
meson compile
```
