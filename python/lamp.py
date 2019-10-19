import numpy as np
import mppy
import sys


input_nd_file = "../data/iris.5d"
output_2d_file = "../data/iris_5d_out.2d"
#input_nd_file = "../../Data/Heads/heads.nd"
#output_2d_file = "../../Data/Heads/heads.2d"
#input_nd_file = "../../Data/Primitives/primitives.nd"
#output_2d_file = "../../Data/Primitives/primitives.2d"
#input_nd_file = "G:/Data/Heads/heads_csharp.nd"
#output_2d_file = "G:/Data/Heads/heads_csharp.2d"
#input_nd_file = "F:/Projects/DSc/Data/Test/Heads.nd"
#output_2d_file = "F:/Projects/DSc/Data/Test/Heads.2d"


# Check if we have input arguments from command line
if (len(sys.argv) > 2):
    output_2d_file = str(sys.argv[2])
if (len(sys.argv) > 1):
    input_nd_file = str(sys.argv[1])

print("- Input  : ", input_nd_file )
print("- Output : ", output_2d_file )


print("- Loading input file... ")
#data = np.loadtxt(input_nd_file, delimiter=",")
data = np.loadtxt(input_nd_file)

print("- Computing... ")
matrix_2d = mppy.force_2d(data)

#print("- Plotting... ")
#mppy.simple_scatter_plot(matrix_2d)

print("- Saving result... ")
#np.savetxt(output_2d_file, matrix_2d, fmt='%10.5f', delimiter=",")
np.savetxt(output_2d_file, matrix_2d, fmt='%.5f')


print("- Finished! ")
