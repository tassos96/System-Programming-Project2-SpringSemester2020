////////////////////////////////////////////////////////////////////////////////

Προγραμματισμός Συστήματος								  
Εαρινό Εξάμηνο 2019-2020								  

Εργασία 2	

////////////////////////////////////////////////////////////////////////////////

Όνομα: Αναστάσιος
Επώνυμο: Αντωνόπουλος
Α.Μ.: 1115201400014

////////////////////////////////////////////////////////////////////////////////

Η εργασία υλοποιήθηκε με την χρήση του Visual Studio Code σε σύστημα Ubuntu.

Εντολή μεταγλώττισης: 

	make

Εντολή διαγραφής εκτελέσιμου: 

	make clean

Παράδειγμα εντολής εκτέλεσης script: 

	./create_infiles.sh diseasesFile countriesFile input_dir 5 50
	
Παράδειγμα εντολής εκτέλεσης: 

	./diseaseAggregator -w 3 -b 50 -i ./input_dir

////////////////////////////////////////////////////////////////////////////////

Τα αρχεία της εργασίας έχουν οργανωθεί με τον εξής τρόπο:

-Φάκελος diseaseAggregator:

	Περιέχει τα αρχεία "countriesFile", "diseasesFile" καθώς και το script "create_infiles.sh"
	το οποίο τα χρησιμοποιεί για να παράγει το Dir των input files. Επιπλέον περιέχει και το Makefile για την
	μεταγλώττιση του προγράμματος diseaseAggregator. Τέλος περιέχει και τους φακέλους που αναφέρονται παρακάτω.

-Φάκελος log_files:

	Σε αυτόν τον φάκελο θα δημιουργηθούν τα log_files από τους workers και τον πατέρα.

-Φάκελος named_pipes:

	Σε αυτόν τον φάκελο θα δημιουργηθούν τα αρχεία των named pipes για την επικοινωνία μεταξύ
	του πατέρα και των workers.

-Φάκελος odj:

	Εδώ τοποθετούνται τα object files που δημιουργούνται κατά την μεταγλώττιση.

-Φάκελος src:

	Εδώ περιέχονται όλα τα source files του προγράμματος και ο φάκελος include που περιέχει τα 
	αντίστοιχα header files.

Στον κώδικα της εργασίας περιέχονται σχόλια για την λειτουργικότητα του προγράμματος.

////////////////////////////////////////////////////////////////////////////////

Ο κώδικας έχει οργανωθεί ως εξής:

	-main.c
		Περιέχει την main function του προγράμματος.

	-cmdArguments.c
		Περιέχει functions οι οποίες χρησιμοποιούνται για το διάβασμα των παραμέτρων του προγράμματος
		από την εντολή εκτέλεσης.

	-fileOperations.c
		Περιέχει functions που αφορούν το διάβασμα των Directories από το input_file, το διάβασμα των αρχείων
		καθώς και την εισαγωγή των records από τα αρχεία στις δομές δεδομένων.

	-avlTree.c / hashTable.c / hashTablesBucket.c / hashTablesBucketEntry.c / record.c / recordsList.c
		Είναι τα ίδια από την πρώτη εργασία. Περιέχουν μεθόδους για την λειτουργικότητα των δομών 
		που γεμίζουν από τα records των input files.

	-namedPipes.c
		Περιέχει functions για την δημιουργία των named pipes αλλά και την διαγραφή τους κατά το τέλος
		της εκτέλεσης του προγράμματος.

	-stat.c / statsList.c
		Περιέχει μεθόδους για την αποθήκευση των statistics από τους workers που λαμβάνει ο πατέρας σε μία συνδεδεμένη λίστα 
		αλλά και για τον υπολογισμό κάποιων ερωτημάτων στα οποία μπορεί να απαντήσει ο πατέρας μέσω αυτών των statistics.

	-workerOperations.c
		Περιέχει όλα τα operations που κάνει ο κάθε worker.

	-parentOperations.c
		Περιέχει όλα τα operations που κάνει ο πατέρας.

	-interface.c
		Υλοποίηση των ερωτημάτων τα οποία θέτει ο χρήστης και πρέπει να απαντήσει ο πατέρας.

	-myDateStructure.c
		Ίδιο με την πρώτη εργασία. Υλοποίηση βοηθητικής struct για τον χειρισμό των ημερομηνιών.

////////////////////////////////////////////////////////////////////////////////

Συνοπτική Περιγραφή

Αρχικά το πρόγραμμα διαβάζει τα arguments που δόθηκαν απο την γραμμή εντολής κατά
την εντολή εκτέλεσης. Αν υπάρχει κάποιο σφάλμα το πρόγραμμα θα τερματιστεί και θα 
εκτυπώσει το αντίστοιχο μήνυμα.

Στην συνέχεια θα δημιουργήσει με βάση τον αριθμό των workers τα αντίστοιχα named pipes.
Ύστερα θα προχωρήσει στην κλήση της fork για την δημιουργία όλων των workers.

Οι workers θα ανοίξουν τα αντίστοιχα named pipes και στην συνέχεια θα αρχίσουν να εκτελούν τις προκαθορισμένες
λειτουργικότητες τους.
Οι workers θα τερματίσουν είτε αν δοθεί από τον χρήστη η εντολή /exit είτε αν ο πατέρας λάβει τα αντίστοιχα
σήματα τερματισμού που αναφέρει η εκφώνηση.

Ο πατέρας θα ανοίξει και αυτός τα αντίστοιχα named pipes και θα αρχίσει και αυτός να εκτελεί τις προκαθορισμένες
λειτουργικότητες τους.

////////////////////////////////////////////////////////////////////////////////

Ακολουθεί ενδεικτική εκτέλεση του create_infiles.sh :

	Create directory for 'Denmark'...
	Success
	Create input files for 'Denmark'...
	Success 

	Create directory for 'France'...
	Success
	Create input files for 'France'...
	Success 

	Create directory for 'Vietnam'...
	Success
	Create input files for 'Vietnam'...
	Success 

	Create directory for 'Argentina'...
	Success
	Create input files for 'Argentina'...
	Success 

	Create directory for 'Switzerland'...
	Success
	Create input files for 'Switzerland'...
	Success 

	Create directory for 'Greece'...
	Success
	Create input files for 'Greece'...
	Success 

	Create directory for 'Egypt'...
	Success
	Create input files for 'Egypt'...
	Success 

	Create directory for 'China'...
	Success
	Create input files for 'China'...
	Success 

	Create directory for 'Germany'...
	Success
	Create input files for 'Germany'...
	Success 

	Create directory for 'Australia'...
	Success
	Create input files for 'Australia'...
	Success 

	Create directory for 'Russia'...
	Success
	Create input files for 'Russia'...
	Success 

	Create directory for 'Italy'...
	Success
	Create input files for 'Italy'...
	Success 

	Create directory for 'USA'...
	Success
	Create input files for 'USA'...
	Success 

	Construction of all input files completed succesfully

////////////////////////////////////////////////////////////////////////////////

Ακολουθεί ενδεικτική εκτέλεση του diseaseAggregator :

	Num of workers: 2
	Buffer Size: 50
	Input Dir Path: ./input_dir
	Parent --> pid: 18339 
	Worker 0 -->pid = 18351 and parentId: 18339
	Worker 1 -->pid = 18352 and parentId: 18339
	ERROR 	//<=== Error messages για την ορθότητα των records. Πχ exit record χωρίς να υπάρχει αντίστοιχο entry
	ERROR
	.....
	.....
	ERROR

	Ready for queries

	/listCountries
	Australia 18352
	Argentina 18351
	China 18351
	Egypt 18351
	Germany 18351
	Italy 18351
	Switzerland 18351
	Vietnam 18351
	Denmark 18352
	France 18352
	Greece 18352
	Russia 18352
	USA 18352

	/diseaseFrequency COVID-2019 1-1-2014 31-12-2025
	236

	/diseaseFrequency COVID-2019 1-1-2014 31-12-2025 Argentina
	10

	/topK-AgeRanges 3 Argentina COVID-2019 1-1-2014 31-12-2025
	60+: 50%
	21-40: 40%
	0-20: 10%

	/searchPatientRecord 1
	1 FirstName13 LastName2 MERS-COV 91 30-08-2019 --

	/numPatientAdmissions COVID-2019 1-1-2014 31-12-2025
	Argentina 10
	Australia 12
	China 16
	Denmark 20
	Egypt 23
	France 22
	Germany 17
	Greece 24
	Italy 22
	Russia 11
	Switzerland 17
	USA 25
	Vietnam 17

	/numPatientAdmissions COVID-2019 1-1-2014 31-12-2025 Argentina
	Argentina 10

	/numPatientDischarges COVID-2019 1-1-2014 31-12-2025
	Argentina 0
	Australia 0
	China 0
	Denmark 0
	Egypt 0
	France 0
	Germany 0
	Greece 0
	Italy 0
	Russia 0
	Switzerland 0
	USA 0
	Vietnam 0

	/numPatientDischarges COVID-2019 1-1-2014 31-12-2025 Argentina
	Argentina 0

	/exit
	Worker process terminated successfully
	Worker process terminated successfully
	Parent terminated successfully

////////////////////////////////////////////////////////////////////////////////
