import serial
import math
import open3d as o3d
import numpy as np # needed to feed into Open3D methods

s = serial.Serial("COM4", 115200) # open serial port

print("Opening:", s.name)

# number of z-axis measurements
TOTAL_Z_STEPS = 3
# x/y plane measurements per rotation
SAMPLES = 64

vals = []
f = open("./data.xyz", "w") # open file

for z in range(TOTAL_Z_STEPS):  #to be incremented with each step
    print(f"Measurement {z+1} of {TOTAL_Z_STEPS}")

    for i in range(SAMPLES):
        r = 0
        s.write(b'1')
        while 1:
            inChar = s.read()        # read one byte
            c = inChar.decode()      # convert byte type to str
            if c == ",":
                break # comma means end of measurement
            else:
                r = int(str(r) + str(c)) # concatenation


        angle = i*(360.0/SAMPLES) 
        x = round(r * math.cos((math.pi/180.0) * angle), 1) # in mm
        y = round(r * math.sin((math.pi/180.0) * angle), 1) # in mm
        
        vals.append([x, y, 200*z]) # z val needs to be the number of mm/step
        
    
print("Closing: " + s.name)

for a in range(SAMPLES*TOTAL_Z_STEPS): # angular measurements times z measurements
    string = '%s %s %s\n' % (vals[a][0], vals[a][1], vals[a][2]) # .xyz format
    print(string)
    f.write(string)

s.close()
f.close() # cleanup



### VISUALIZE IN OPEN3D


pcd = o3d.io.read_point_cloud("./data.xyz", format='xyz')

lines = []

# create loops
for z in range(TOTAL_Z_STEPS):

    offset = SAMPLES * z

    for i in range(SAMPLES):
        if i < SAMPLES - 1:
            lines.append([i + offset, i+1 + offset])
        else:
            lines.append([i + offset, offset])


# connect the loops
for z in range(TOTAL_Z_STEPS-1):
    startIndex = z*SAMPLES
    nextStartIndex = (z+1)*SAMPLES

    for i in range(SAMPLES):
        lines.append([startIndex+i, nextStartIndex+i])


pts = np.asarray(pcd.points)
print(pts)


line_set = o3d.geometry.LineSet(
    points=o3d.utility.Vector3dVector(pts),
    lines=o3d.utility.Vector2iVector(lines),
)

o3d.visualization.draw_geometries([line_set], width=1280, height=720)