#!/bin/bash

#FUNCTIONS DECLERATIONS
day=0
month=0
year=0
peek_date() #Function to peek random date
{
    #Choose random day
    day=$((1 + RANDOM % 30))

    #Choose random month
    month=$((1 + RANDOM % 12))

    #Choose random year
    MINIMUM=2018; #minimum year
    MAXIMUM=2020; #maximum year
    RANGE=$(($MAXIMUM-$MINIMUM+1));

    RESULT=$RANDOM;
    let "RESULT %= $RANGE";
    RESULT=$(($RESULT+$MINIMUM));
    year=$RESULT
}

currentRecord=0
randomIndex=0
create_record() #Function to construct a record entry
{
    randomIndex=$((RANDOM % $STATUSES_SIZE))
    status="${STATUSES[$randomIndex]} " #Add record status

    if [ "$status" == "EXIT" ] #If status == EXIT then you can use already used id
        then
            id="$((1 + RANDOM % $TOTAL_RECORDS)) "  #Take random id between ids range
        else
            id="${SHUFFLED_IDS[$currentRecord]} "  #Take id from shuffled id array
    fi

    randomIndex=$((RANDOM % $FIRST_NAMES_SIZE))
    firstName="${FIRST_NAMES[$randomIndex]} " #Add record first name

    randomIndex=$((RANDOM % $LAST_NAMES_SIZE))
    lastName="${LAST_NAMES[$randomIndex]} " #Add record last name

    randomIndex=$((RANDOM % $diseasesArraySize))
    disease="${diseasesArray[$randomIndex]} " #Add record disease

    age="$((1 + RANDOM % 120)) " #Add record age

    recordString+="$id $status $firstName $lastName $disease $age "

    currentRecord=$((currentRecord+1))  #Move index to next id
}

# ------------------  MAIN SCRIPT ---------------------------

#Check for arguments number
NEEDED_ARGS=5
if [ $NEEDED_ARGS -ne $# ]; then
    echo "Arguments error: 5 arguments expected."
    exit 1
fi

#Check if input diseasesFile exist
if [ ! -f  $1 ]; then
    echo "Arguments error: FIle '$1' does not exist"
    exit 1
fi

#Check if input countriesFile exist
if [ ! -f $2 ]; then
    echo "Arguments error: File '$2' does not exist"
    exit 1
fi

#Store Arguments $4 $5
NUM_FILES_PER_DIRECORY=$4
NUM_RECORDS_PER_FILE=$5

#Create directory from argument
mkdir -p $3 || { echo 'Error.' >&2; exit 1; }

#Read and store to a string array the content of diseasesFile
diseaseString=$(cat $1 | tr "\n" " ")
diseasesArray=($diseaseString)
diseasesArraySize=${#diseasesArray[@]}

#Read and store to a string array the content of countriesFile
countryString=$(cat  $2 | tr "\n" " ")
countriesArray=($countryString)
countriesArraySize=${#countriesArray[@]}

#Variables for records generator
TOTAL_RECORDS=$(( $countriesArraySize * $NUM_FILES_PER_DIRECORY * $NUM_RECORDS_PER_FILE ))
IDS=$(seq 1 $TOTAL_RECORDS)

SHUFFLED_IDS=( $(echo "${IDS[@]}" | tr " " "\n" | shuf | tr -d " " ) ) #shuffle ids to random order


STATUSES=( "ENTER" "EXIT" )
STATUSES_SIZE=${#STATUSES[@]}

FIRST_NAMES=( "FirstName1" "FirstName2" "FirstName3" "FirstName4" "FirstName5" "FirstName6" "FirstName7" "FirstName8" "FirstName9" "FirstName10" 
              "FirstName11" "FirstName12" "FirstName13" "FirstName14" "FirstName15" "FirstName16" "FirstName17" "FirstName18" "FirstName19" "FirstName20" )
FIRST_NAMES_SIZE=${#FIRST_NAMES[@]}

LAST_NAMES=( "LastName1" "LastName2" "LastName3" "LastName4" "LastName5" "LastName6" "LastName7" "LastName8" "LastName9" "LastName10" 
             "LastName11" "LastName12" "LastName13" "LastName14" "LastName15" "LastName16" "LastName17" "LastName18" "LastName19" "LastName20" )
LAST_NAMES_SIZE=${#LAST_NAMES[@]}

#Go into input_dir directory
cd $3 || { echo 'Error.' >&2; exit 1; }

#Create directories for each country
for string in "${countriesArray[@]}"
    do
        echo "Create directory for '$string'..."   
        mkdir -p $string || { echo 'Error.' >&2; exit 1; }
        echo "Success"
        #For each directory created, make 'NUM_FILES_PER_DIRECORY' files
        cd $string || { echo 'Error.' >&2; exit 1; }
        echo "Create input files for '$string'..."
        for i in $(seq 1 $NUM_FILES_PER_DIRECORY)
            do
                peek_date   #call function to create a date
                dateFileName="$(printf %02d $day)-$(printf %02d $month)-$year"
                if [ ! -e $dateFileName ]; then
                    #Add records to file
                    for i in $(seq 1 $NUM_RECORDS_PER_FILE)
                    do
                        recordString="" #String to append at file
                        create_record   #call function to create a record
                        echo $recordString >> $dateFileName #Append record to file
                        recordString="" #Empty string
                    done
                fi
            done
        #END OF FILES CREATION
        
        printf "Success \n\n"
        cd .. || { echo 'Error.' >&2; exit 1; }
    done

#Return to parent directory
cd .. || { echo 'Error.' >&2; exit 1; }

printf "Construction of all input files completed succesfully\n\n"
exit 0