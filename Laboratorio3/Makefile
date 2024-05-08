connect:
	socat -d -d pty,raw,echo=0,link=/tmp/ttyS0 pty,raw,echo=0,link=/tmp/ttyS1

print:
	# rm Tensiones.csv
	python3 datasaver.py

prepare:
	sudo apt install socat
	pip install pyserial
