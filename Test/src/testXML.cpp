/**
 * XML testing
 * @author Shiwei Zhang
 * @date 2014.01.10
 */

#include "stdafx.h"

#include <iostream>
#include <exception>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

#pragma comment(lib, "tinyxml2")

void testPrintXML();
void testErrMsg();
void testGame();

int testXML() {
	try {
		testGame();
		//testErrMsg();
		//testPrintXML();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}

	system("pause");
	return 0;
}

void testGame()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("game.xml");
	if (doc.ErrorID())
		return ;
	
	tinyxml2::XMLElement* root;
	tinyxml2::XMLElement* node;
	tinyxml2::XMLElement* croot;
	tinyxml2::XMLElement* cnode;
	tinyxml2::XMLElement* droot;
	tinyxml2::XMLElement* dnode;

	root = doc.FirstChildElement("root");
	
	if (root == NULL)
	{
		return;
	}
}

void testErrMsg() {
	XMLDocument doc;
	XMLElement* root;
	XMLElement* node;
	doc.InsertEndChild(root = doc.NewElement("root"));
	root->InsertEndChild(node = doc.NewElement("status"));
	node->InsertEndChild(doc.NewText("1"));
	root->InsertEndChild(node = doc.NewElement("error"));
	node->InsertEndChild(doc.NewText("hello world"));

	XMLPrinter printer(0, true);
	doc.Print(&printer);
	cout << printer.CStr() << endl;
	cout << printer.CStrSize() << endl;
	cout << strlen(printer.CStr()) << endl;
}

void testPrintXML() {
	XMLDocument doc;
	XMLElement* root = doc.NewElement("root");
	XMLElement* path = doc.NewElement("status");
	XMLText* text = doc.NewText("0");
	doc.InsertEndChild(root);
	root->InsertEndChild(path);
	path->InsertEndChild(text);
	XMLPrinter printer(0, true);
	doc.Print(&printer);
	cout << printer.CStr() << endl;
}
