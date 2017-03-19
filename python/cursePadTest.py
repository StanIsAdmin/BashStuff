from cursePad import *


def testBasicFunctions():
	#Pad creation
	pad = Pad("Hello\n\nHola\n")
	assert(str(pad) == "Hello\n\nHola\n")
	
	#Cursor creation
	cur_id = pad.cursor_create()
	cur_ref = pad._get_cursor_from_id(cur_id)
	assert(cur_ref is not None)
	
	#Cursor seek
	pad.cursor_seek(cur_id, 5, "llo\n\nH", 3) #end of hello
	assert(cur_ref.row == 0 and cur_ref.col == 5)
	
	#Insertion
	pad.insert(cur_id, " to")
	pad.insert(cur_id, " you")
	assert(cur_ref.row == 0 and cur_ref.col == 12)
	assert(str(pad) == "Hello to you\n\nHola\n")
	
	#Removal
	pad.remove(cur_id, len(" you"))
	assert(cur_ref.row == 0 and cur_ref.col == 8)
	assert(str(pad) == "Hello to\n\nHola\n")
testBasicFunctions()

def testTwoCursors():
	pad = Pad("Hello\n\nHola\n")
	cur1_id = pad.cursor_create()
	cur2_id = pad.cursor_create()
	
	#Multi-seek
	pad.cursor_seek(cur1_id, 5, "llo\n\nH", 3) #end of hello
	pad.cursor_seek(cur2_id, 4, "ola", 3) #end of hola
	
	#Multi-insert
	pad.insert(cur1_id, "\nDear Fella !")
	pad.insert(cur2_id, "\nCómo estás ?")
	assert(str(pad) == "Hello\nDear Fella !\n\nHola\nCómo estás ?\n")
	
	#Multi-remove
	pad.remove(cur1_id, 15)
	assert(str(pad) == "Hel\n\nHola\nCómo estás ?\n")
	pad.remove(cur2_id, 4)
	assert(str(pad) == "Hel\n\nHola\nCómo est\n")
testTwoCursors()
