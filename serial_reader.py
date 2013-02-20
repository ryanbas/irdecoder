import serial
import sys

def main():
    if len(sys.argv) == 2:
        signature = "void " + sys.argv[1] + "() {"
    else:
        print "Bad number of args"
        return

    s = serial.Serial('/dev/tty.usbserial-A800eHrB', 9600)
#while True:
#    line = s.readline().strip()
#    if line == 'READY':
#        break

    num_samples = 5
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

    ons = lambda pulses: [p[0] for p in pulses]
    offs = lambda pulses: [p[1] for p in pulses]
    average = lambda l: sum(l)/num_samples
    nearestTen = lambda num: num#((num + 5) / 10) * 10


    print signature
    for pulses in zipped:
        #print(pulses)
        on_average = average(ons(pulses))
        off_average = average(offs(pulses))

        call = "  onOff(" + str(nearestTen(on_average)) + ", " + str(nearestTen(off_average)) + ");"
        print call

    print "}"

main()
