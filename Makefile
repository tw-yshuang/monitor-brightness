TARGET=monitor-brightness
OBJ=monitor-brightness.o
Monitor=$(shell xrandr | grep " connected " | awk '{ print$$1 }') # Change here!! if you have specific monitor name.
BIN_PATH=/usr/local/bin/
AUTO_START_PATH=/usr/share/gnome/autostart/

all: install clean

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BIN_PATH)
	install -m 755 $(TARGET) $(DESTDIR)$(BIN_PATH)
	mkdir -p $(DESTDIR)$(AUTO_START_PATH)
	sed -i "s/Exec=$(TARGET)/& $(Monitor)/" $(TARGET).desktop
	install -m 644 $(TARGET).desktop $(DESTDIR)$(AUTO_START_PATH)

.PHONY: clean uninstall
clean:
	rm -f $(TARGET) $(OBJ)

uninstall:
	rm -rf $(DESTDIR)$(AUTO_START_PATH)$(TARGET).desktop
	rm -rf $(DESTDIR)$(BIN_PATH)$(TARGET)

