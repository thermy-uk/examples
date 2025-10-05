.PHONY: build deploy attach

build:
	arduino-cli compile \
		--output-dir output/ \
		--fqbn esp32:esp32:esp32 .

deploy:
	arduino-cli upload -p /dev/tty.usbserial-0001 --input-dir output/ --fqbn esp32:esp32:esp32 .

attach:
	screen /dev/tty.usbserial-0001 115200
