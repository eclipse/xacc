import xacc

# Load the AcceleratorBuffer
buffer = xacc.loadBuffer(open('ddcl_example_data.ab', 'r').read())

# Print it to see it all as JSON
print(buffer)

# You can get all unique information at given key,
# here let's get all unique parameter sets the problem
# was run at
parameters = buffer.getAllUnique('parameters')

# Loop over those parameters
for p in parameters:
    # print them to see
    print(p)

    # Now, you can get all children buffers
    # that correspond to those parameters
    childrenAtParams = buffer.getChildren('parameters',p)

    # Loop over those to see all counts dictionaries 
    for c in childrenAtParams:
        print(c.name(), c.getMeasurementCounts())