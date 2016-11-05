#!/bin/bash

#Args are : path to work folder, path to backup folder
PATH_WRK=$1
PATH_BAK=$2

#If paths are relative, convert them to absolute
if [[ $PATH_BAK != [/~]* ]]; then PATH_BAK="${PWD}${PATH_BAK}"; fi
if [[ $PATH_WRK != [/~]* ]]; then PATH_WRK="${PWD}${PATH_WRK}"; fi
echo "Backup path : $PATH_BAK"
echo "Work path   : $PATH_WRK"

#Timestamp for backup (seconds since epoch)
TIME_NOW=`date "+%s"`

#Check if there is a work directory
if [ ! -d $PATH_WRK ]; then
	exit
fi


#Check if there is a backup directory
if [ ! -d $PATH_BAK ]; then
	mkdir $PATH_BAK
fi

#Iterate over file in work folder
for CURRENT_PATH_WRK in $(find $PATH_WRK); do
	
	#Get path of matching file in backup folder
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

			#If work file was modified since last backup
			if [ "$TIME_BAK" -lt "$TIME_WRK" ]; then
				
				#Copy file to backup
				cp --preserve=timestamps $CURRENT_PATH_WRK $CURRENT_PATH_BAK > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					echo "Copied modified file ${CURRENT_PATH_WRK#PATH_WRK}"
				else
					echo "Error: could not copy modified file ${CURRENT_PATH_WRK#PATH_WRK}"
				fi
			fi
		
		#If the backup does not contain the file
		else
			#Copy file to backup
			cp --preserve=timestamps $CURRENT_PATH_WRK $CURRENT_PATH_BAK > /dev/null 2>&1
			if [ $? -eq 0 ]; then
				echo "Copied new file ${CURRENT_PATH_WRK#PATH_BAK}"
			else
				echo "Error: could not copy new file ${CURRENT_PATH_WRK#PATH_WRK}"
			fi
		fi
	fi
done