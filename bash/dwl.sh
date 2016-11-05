#!/bin/bash

#execute : bash test.sh

#link format : path(relative)%url (no quotes)


while read line; do

	varPATH=$(echo $line | cut -f1 -d%)

	varURL=$(echo $line | cut -f2 -d%)

	echo $varPATH

	echo $varURL

	wget -c "${varURL}" -O "vids/${varPATH}"

done < $1
#-P : choose directory


#-A : Accept extensions ...

#-r : recursive search

#-H : go to exterior links (necessary)

#-l : recursion depth (1)
#-np : no parent search

#-nd : no directory structure from url
#-e robots=off : (?)

#Later : discard already downloaded



#PARALLEL
#entry format : filename%url (no quotes)


entryfile=$1

while read line; do

	varPATH="vids/$(echo $line | cut -f1 -d%)"

	varURL=$(echo $line | cut -f2 -d%)

	echo "-c \"$varURL\" -O \"$varPATH\""
done < $entryfile | xargs -n 4 -P 4 wget -nv
#wget -c "${varURL}" -O "vids/${varPATH}"
#| : pipe

#xargs -n : number of arguments per wget command
#xargs -P : simultaneous processes
#wget -c : continue
#wget -O : output filename
#wget -q : silent output

#!/bin/bash

#execute : bash test.sh


#link format : <relative path>%<url>


while read line; do

	varPATH=$(echo $line | cut -f1 -d%)

	varURL=$(echo $line | cut -f2 -d%)

	echo $varPATH

	echo $varURL
	wget -P "${varPATH}" -A "gif,webm,jpeg,jpg,png" -rH -l 1 -np -nd -e "robots=off" "${varURL}"

	find "${varPATH}" -type f -size -10k -delete
done < $1


#-P : choose directory

#-A : Accept extensions ...
#-r : recursive search

#-H : go to exterior links (necessary)

#-l : recursion depth (1)

#-np : no parent search

#-nd : no directory structure from url

#-e robots=off : (?)

#Later : discard already downloaded
