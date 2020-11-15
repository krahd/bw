import serial, time

arduino = serial.Serial('COM3', 9600, timeout=.1)

time.sleep(1) #give the connection a second to settle

f = 200.54
arduino.write(str(f).encode())

while True:
	data = arduino.readline()

	if data:
		print (data.rstrip('\n')) #strip out the new lines for now
		# (better to do .read() in the long run for this reason


