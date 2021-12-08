#!/bin/bash

set -u


LOG_FILE="log.txt"
BANNER_FILE="banner.txt"

ROOT_FOLDER="../"
UPDATED_EXTENSIONS=".h .c .cpp"


# Removes the previous header from a file.
# Before that, checks if there was a header present in the file or not
#+by checking if the first line starts by /***********************
function removeHeader {
    local fileName=$1


    #checks if the file contains a banner
    local out=$(sed -nr '/^\/\*+$/p;q' $fileName)
    if [[ ! $out ]]; then
	echo -e "\t[WARNING] <$fileName> does not contain a banner ..." >> $LOG_FILE
	return
    fi


    # remove banner
    sed -ri '0,/^\s+\*+\/$/d' $fileName
    echo -e "\t[OK] <$fileName>: banner removed" >> $LOG_FILE
}




# Add the banner $BANNER_FILE to thebeginning of the file
function prependBanner {
    local fileName=$1

    local tmpFile=$(mktemp)


    cat "$BANNER_FILE" "$fileName" > "$tmpFile"
    mv "$tmpFile" "$fileName"


    echo -e "\t<$fileName> updated" >> $LOG_FILE
}



# Makes a backup copy of the folder specified in $ROOT_FOLDER
function backUp {

    local defaultPath="/tmp/3d-ice_backup/"
    local backupFolder=""
    
    read -p "Enter a path to make a backup before starting [$defaultPath]: " backupFolder
    backupFolder=${backupFolder:-$defaultPath}

    echo -n "Performing a back-up in $backupFolder .....   " >> $LOG_FILE

    mkdir -p $backupFolder

    cp -r "$ROOT_FOLDER" "$backupFolder"

    echo "DONE!"
    echo "DONE!" >> $LOG_FILE
}






function Main {


    echo "Starting the process. A log file will be created in: $LOG_FILE"

    backUp

    echo "Updating the banner ..."
    echo "Updating the banner" >> $LOG_FILE
    


    
    for ext in $UPDATED_EXTENSIONS; do
	local files=$(find $ROOT_FOLDER -name "*${ext}" -exec echo {} \;)
	for f in $files; do
	    echo "Updating $f"

	    
	    removeHeader $f
	    prependBanner $f
	done
    done
}


Main
