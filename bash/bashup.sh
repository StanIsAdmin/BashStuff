#!/bin/bash

#Args are : path to work folder, path to backup folder
PATH_WRK=$1
PATH_BAK=$2

function fixpath {
	NEWPATH=$1
	#If paths are relative, convert them to absolute
	if [ $NEWPATH == ".." ]; then NEWPATH="$(dirname "$(pwd)")"; 
	elif [ $NEWPATH == "." ]; then NEWPATH="${PWD}";
	elif [ $NEWPATH != [/~]* ]; then NEWPATH="${PWD}${NEWPATH}" ; fi
	echo "${NEWPATH}"
}

PATH_WRK="$(fixpath ${PATH_WRK})"
PATH_BAK="$(fixpath ${PATH_BAK})"
echo "Work path   : $PATH_WRK"
echo "Backup path : $PATH_BAK"

COPYCOUNT=0
IGNORECOUNT=0
FAILCOUNT=0

#Timestamp for backup (seconds since epoch)
TIME_NOW=`date "+%s"`

#Check if there is a work directory, exit if not
if [ ! -d $PATH_WRK ]; then
	echo 
	exit
fi


#Check if there is a backup directory, make it if not
if [ ! -d $PATH_BAK ]; then
	mkdir $PATH_BAK
fi

#Iterate over paths in work folder
for CURRENT_PATH_WRK in $(find $PATH_WRK); do
	
	#Get matching path in backup folder
	CURRENT_PATH_BAK="${PATH_BAK}${CURRENT_PATH_WRK#$PATH_WRK}"
	
	#If the current work path is a directory
	if [ -d $CURRENT_PATH_WRK ]; then
		
		#If the matching backup path does not exist, create it
		if [ ! -d $PATH_BAK_FILE ]; then
			mkdir $PATH_BAK_FILE
		fi		
	fi

	#If the current work path is a file
	if [ -f $CURRENT_PATH_WRK ]; then
		
		#If the file exists in the backup
		if [ -f $CURRENT_PATH_BAK ]; then
			#Modification time of work and backup files (seconds since epoch)
			TIME_WRK=$(stat --format=%Y $CURRENT_PATH_WRK)
			TIME_BAK=$(stat --format=%Y $CURRENT_PATH_BAK)

			#If work file was modified since last backup, copy it
			if [ "$TIME_BAK" -lt "$TIME_WRK" ]; then
				
				#Copy file to backup, ignore outputs (use ? for return value of command)
				cp --preserve=timestamps $CURRENT_PATH_WRK $CURRENT_PATH_BAK > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					echo "Copied modified file ${CURRENT_PATH_WRK#PATH_WRK}"
					(( COPYCOUNT++ ))
				else
					echo "Error: could not copy modified file ${CURRENT_PATH_WRK#PATH_WRK}"
					(( FAILCOUNT++ ))
				fi
			else
				(( IGNORECOUNT++ ))
			fi
		
		#If the file does not exist in the backup, copy it
		else
			cp --preserve=timestamps $CURRENT_PATH_WRK $CURRENT_PATH_BAK > /dev/null 2>&1
			if [ $? -eq 0 ]; then
				echo "Copied new file ${CURRENT_PATH_WRK#PATH_BAK}"
				(( COPYCOUNT++ ))
			else
				echo "Error: could not copy new file ${CURRENT_PATH_WRK#PATH_WRK}"
				(( FAILCOUNT++ ))
			fi
		fi
	fi
done

echo "...done (${COPYCOUNT} copied, ${IGNORECOUNT} ignored, ${FAILCOUNT} failed)"