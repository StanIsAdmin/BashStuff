import os, hashlib

def getFilePaths(directory):
	"""
	Returns all files within a directory and its subdirectories
	"""
	for folder, subs, files in os.walk(directory):
		for filename in files:
			yield os.path.join(folder, filename)
			
	

def hashFiles(directory):
	"""
	Returns a dictionary mapping hexdigest of files in dir to their paths.
	"""
	filedict = {}
	for path in getFilePaths(directory):
		with open(path, 'rb') as file:
			filedict[hashlib.md5(file.read()).hexdigest()] = path
	return filedict

