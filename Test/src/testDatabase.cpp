/**
 * Database testing
 * @author Shiwei Zhang
 * @date 2014.02.008
 */

#include <iostream>
#include "yiqiding/ktv/db/Database.h"
using namespace yiqiding::ktv::db;
using namespace std;

void testMediaConnector();
void testMediaTypeId();
void testGetActor();
void testUpdateMediaList();

Database db;
MediaConnector* mc = db.getMediaConnector();

int main() {
	try {
		db.setLogin("localhost", "yiqiding", "ktv_dev", "yiqiding_ktv");
		//testMediaConnector();
		//testMediaTypeId();
		//testGetActor();
		testUpdateMediaList();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	system("pause");
	return 0;
}

void testUpdateMediaList() {
	std::vector<unsigned int> sids;
	sids.push_back(1478);
	sids.push_back(10);
	sids.push_back(2573);
	sids.push_back(9498);
	mc->updateMediaList("so898", sids);
}

void testGetActor() {
	auto actor = mc->getActor(10715);
	cout << actor->getName() << endl;
	cout << actor->getName().size() << endl;
}

void testMediaTypeId() {
	cout << mc->getMediaTypeId("couple") << endl;
}

void testMediaConnector() {
	model::Media media;
	media.setName("²âÊÔ");
	mc->add(media);
}
