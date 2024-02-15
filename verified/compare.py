import sys
import os
import filecmp

dir1 = sys.argv[1]
dir2 = sys.argv[2]

#for a file, write to a new file with whitespace excluded

def remove_whitepace(file_in, file_out):
	lines = []
	with open(file_in, 'r') as f:
		lines = f.readlines()

	#strips all whitespace from string
	lines = [''.join(line.split()) for line in lines]

	#annoying - filepath appears in debug info, need to strip it
	#pretty bad hack - should use os methods I think
	file1 = file_in.replace("verified/build/", "src/")
	file_aux = file1.replace(".i", ".c")

	lines = [line.replace(str(file_aux), "") for line in lines]

	#1 more horrible hack: remove "/temp" everywhere, other filepaths also
	#Really, just want to delete all filepaths - regex?
	lines = [line.replace("/temp", "") for line in lines]

	with open(file_out, 'w') as f:
		f.writelines(lines)

changed = False


for root, dirs, files in os.walk(dir1):
	for filename in files:
		#only keep the .i files
		(name, ext) = os.path.splitext(filename)
		#print(ext)
		if ext != '.i':
			continue
		suffix = os.path.relpath(root, dir1)
		#original file
		path1 = os.path.join(root, filename)
		newpath1 = os.path.join(root, name + "_aux.i")
		#previous file
		root2 = os.path.join(dir2, suffix)
		path2 = os.path.join(root2, filename)
		newpath2 = os.path.join(root2, name + "_aux.i")

		#generate whitespace for each file
		remove_whitepace(path1, newpath1)
		remove_whitepace(path2, newpath2)
		
		#compare files
		c = filecmp.cmp(newpath1, newpath2, shallow=False)

		#delete whitespace files
		os.remove(newpath1)
		os.remove(newpath2)
		#if difference, exit(1)
		if not c:
			print("Difference in file" + filename)
			changed = True

if changed:
	sys.exit(1)
else:
	print("Success: verified subset is identical")
	sys.exit(0)
