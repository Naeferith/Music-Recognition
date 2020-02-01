#include "Database.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "AudioRecorder.h"
#include "ModelFFT.h"

const string Database::SONGEXT = ".wav";

Database* Database::instance = nullptr;

Database::Database()
{
	//Instanciation de la base de donn�e

	//On cherche a trouver 2 fichiers :
	//songs.dat  = Contient la liste des chansons enregistr�es dans la base de donn�e
	//hashes.dat = Contient toutes les emprintes des fichiers audio

	ifstream fsong, fhashes;
	fsong.open("/Database/songs.dat", ios::binary | ios::in); //<--- Intentionnel (marchera pas)
	fhashes.open("Database/hashes.dat", ios::binary | ios::in);

	//On va admettre que si une erreur se fait � l'ouverture des fichiers
	//La BDD est corrompue : -> suppression/troncation des fichiers et reg�n�ration

	if (!fsong || !fhashes) {
		cout << "Failed to open data files." << endl << "Regenerating..." << endl;
		generateDatabase();
	}
	else {
		//Les 2 fichiers sont ouverts : On entamme la d�serialization
		//songs prends ses elements DANS L'ORDRE du fichier songs.dat
		//hashes prends ses elements du fichier hashes.dat
		int i = 0;

		fsong.seekg(0, fsong.end);
		int fileLength = fsong.tellg();
		fsong.seekg(0, fsong.beg);

		while (i < fileLength)
		{
			//string rd;
			songs.emplace_back();
			fsong.read((char *)&songs.back(), sizeof(string));
			i += fsong.gcount();
			//songs.push_back(rd.c_str());
		}

		i = 0;


		//Tout s'est bien pass� : On ferme les fichiers
		fsong.close();
		fhashes.close();
	}
}

void Database::generateDatabase() {
	cout << "Processing files..."<< endl;
	//On ouvre les fichiers de donn�es
	//Il n'existe pas -> Il est cr��
	//Il existe       -> Il est tronqu�
	ofstream fsongs, fhashes;
	fsongs.open("Database/songs.dat", std::ios::binary | std::ios::out | std::ios::trunc);
	fhashes.open("Database/hashes.dat", std::ios::binary | std::ios::out | std::ios::trunc);

	//Erreur lors de la creation, l'application ne marchera pas, on ferme l'application
	if (!fsongs || !fhashes)
		exit(14);

	//On va chercher les chasons dans leur r�pertoire
	int index = 0;
	for (const auto& entry : experimental::filesystem::directory_iterator("Database/Songs")) {
		if (computeSong(entry.path(), index)) index++;
	}
	cout << "Database Succesfully Loaded. Writing in files..." << endl;

	//Ecriture dans songs.dat
	for (auto &song : songs) {
		fsongs.write(song.c_str(), song.size());
	}
	

	//Tout s'est bien pass� : On ferme les fichiers
	fsongs.close();

	if (!fsongs.good()) cout << "Error occurred at writing time!" << endl;

	///
	fhashes.close();
}

bool Database::computeSong(const std::experimental::filesystem::path & path, int songId)
{
	//Le fichier est bien un fichier audio g�r�
	if (path.extension() == SONGEXT) {
		cout << "\t Processing song : " << path.string() << endl;

		AudioRecorder* buffer = AudioRecorder::getInstance();

		buffer->loadBufferFromFile(path.string());
		buffer->computePostProcessing();

		//Le flux audio est pret au hashage
		//On applique une FFT de 1024 sample

		//PROPRIETES DE LA FFT 1024 (echatillonage 11.025 kHz)
		//512 bins
		//10 FFT par secondes
		//Bin large de 10.77 Hz
		ModelFFT fft = ModelFFT(buffer->getPpBuffer(), POW2_10);
		fft.load(hashes, songId);
		songs.emplace_back(path.stem().string());

		return true;
	}
	return false;
}


Database::~Database()
{
}

Database * Database::getInstance()
{
	if (instance == nullptr) instance = new Database();
	return instance;
}

const string & Database::getSongName(int& index) const { return songs[index]; }

const map<long long, vector<DataPoint>> Database::getHashes() const
{
	return hashes;
}

void Database::tryMatch(long long recordHash, int millisOffset)
{
	map<long long, vector<DataPoint>>::iterator it = hashes.find(recordHash);
	if (it != hashes.end()) {

	}
}
