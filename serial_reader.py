import serial
import sys

def main():
    if len(sys.argv) == 2:
        signature = "void " + sys.argv[1] + "() {"
    else:
        print "Bad number of args"
        return

    s = serial.Serial('/dev/tty.usbserial-A800eHrB', 9600)

    num_samples = 3
    samples = []

    print("READY")
    for i in xrange(num_samples):
        sample = []
        line = s.readline().strip()
        prev_on = line.split()[1]
        #print line
        while True:
            line = s.readline().strip()
            #print line
            if line == 'END':
                sample.append((int(prev_on), 0))
                break
            off, new_on = line.split(' ')
            sample.append((int(prev_on), int(off)))
            prev_on = new_on

        samples.append(sample)
        print('RECEIVED')

    s.close()

    averages = []
    zipped = zip(*[sample for sample in samples])

    offs = lambda pulses: [p[1] for p in pulses]
    average = lambda l: sum(l)/num_samples

    bits = ""
    print signature
    for pulses in zipped:
        off_sig = average(offs(pulses))
        bits += "1" if off_sig > 1000 else "0"

    print "  unsigned int code = (B" + bits[1:8] + " << 24) +"
    print "                      (B" + bits[9:16] + " << 16) +"
    print "                      (B" + bits[17:24] + " << 8) +"
    print "                      (B" + bits[25:32] + ");"
    print "  sendCode(code);"
    print "}"

main()
