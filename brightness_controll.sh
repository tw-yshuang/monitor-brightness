#!/usr/bin/env sh

Monitor="eDP-1-1"
blacklight_root="/sys/class/backlight/intel_backlight/"
blacklight_value_root="brightness"
max_blacklight_value_root="max_brightness"

# todo: add notify
# while $(./testc $blacklight_root$blacklight_value_root); do
while current_blacklight_rate=$(./testc $blacklight_root$blacklight_value_root $blacklight_root$max_blacklight_value_root); do
    echo "$current_blacklight_rate"
    CurrBright=$( xrandr --verbose --current | grep -i Brightness | tail -c 5 )
    if [ "$current_blacklight_rate" != "$CurrBright" ]; then
        xrandr --output $Monitor --brightness $current_blacklight_rate
    fi
done 