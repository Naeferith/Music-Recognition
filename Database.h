#pragma once

#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include "DataPoint.h"

using namespace std;

class Database {
private:
	const static string SONGEXT;

	/* On veut consulter qu'une seule BDD -> DP Singleton*/
	static Database* instance;

	Database();

	/* Liste des chansons dans la BDD, index = songID, string = titre*/
	vector<string> songs;

	/* BDD des hashs. Comme un hash est unique, c'est la clé. La valeur est la liste de timestaps ou le hash peut etre trouvé. */
	map<long long, vector<DataPoint>> hashes;

	void generateDatabase();

	void computeSong(const std::experimental::filesystem::path& path, int songId);
public:
	/* Le destructeur.*/
	~Database();

	/* Accesseur sur le singleton.*/
	static Database* getInstance();

	const string& getSongName(int& index) const;

	const map<long long, vector<DataPoint>> getHashes() const;

	void tryMatch(long long recordHash, int millisOffset);
};