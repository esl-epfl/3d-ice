#!/bin/bash

set -u


LOG_FILE="log.txt"

ROOT_FOLDER="../"

BANNER_FILE="banner.txt"
BANNER_FILE_MAKEFILE="banner_makefiles.txt"


UPDATED_EXTENSIONS=".h .c .cpp .l .y"
UPDATED_MAKEFILES="Makefile makefile.def README"



# Removes the previous header from a file.
# Before that, checks if there was a header present in the file or not
#+by checking if the first line starts by /***********************
function removeHeader {
    local fileName=$1


    #checks if the file contains a banner
    local out=$(sed -nr '/^\s*\/\*+$/p;q' $fileName)
    if [[ ! $out ]]; then
	echo -e "\t[WARNING] <$fileName> does not contain a banner ..." >> $LOG_FILE
	return
    fi


    # remove banner
    sed -ri '0,/^\s*\*{2,}\/$/d' $fileName
    echo -e "\t[OK] <$fileName>: banner removed" >> $LOG_FILE
}


# Removes the previous header from a file.
# Before that, checks if there was a header present in the file or not
#+by checking if the first line starts by ############################
function removeHeader_makefile {
    local fileName=$1


    #checks if the file contains a banner
    local out=$(sed -nr '/^\s*#{2,}$/p;q' $fileName)
    if [[ ! $out ]]; then
	echo -e "\t[WARNING] <$fileName> does not contain a banner ..." >> $LOG_FILE
	return
    fi


    # remove banner
    # we call this 2 times to remove first and second ocurrencies
    sed -ri '0,/^\s*#{2,}$/d' $fileName
    sed -ri '0,/^\s*#{2,}$/d' $fileName
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


# Add the banner $BANNER_FILE to thebeginning of the file
function prependBanner_makefile {
    local fileName=$1

    local tmpFile=$(mktemp)


    cat "$BANNER_FILE_MAKEFILE" "$fileName" > "$tmpFile"
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

    echo "Updating the banner of source files ($UPDATED_EXTENSIONS) ..."
    echo "Updating the banner of source files ($UPDATED_EXTENSIONS)" >> $LOG_FILE
    
  
    
    for ext in $UPDATED_EXTENSIONS; do
	local files=$(find $ROOT_FOLDER -name "*${ext}" -exec echo {} \;)
	for f in $files; do
	    echo "Updating $f"

	    
	    removeHeader $f
	    prependBanner $f
	done
    done

    echo "Done!"


    echo "Updating the banner of special files ($UPDATED_MAKEFILES) ..."
    echo "Updating the banner of special files ($UPDATED_MAKEFILES)" >> $LOG_FILE
    
  
    
    for ext in $UPDATED_MAKEFILES; do
	local files=$(find $ROOT_FOLDER -name "${ext}" -exec echo {} \;)
	for f in $files; do
	    echo "Updating $f"

	    
	    removeHeader_makefile $f
	    prependBanner_makefile $f
	done
    done

    echo "Done!"
}


Main
