#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <stack>
#include <string>
#include <vector>
#include "Element.h"

using namespace std;

class Token {
public:
	Token();
	Token(char* content);
	Token(char* content, int type);
	char* name;
	//0 if value
	//1 if startTag
	//2 if endTag
	int tokenType;
};

Token::Token() {
	tokenType = -1;
	char* name = new char[80];
	for (int i = 0; i < 80; i++) {
		name[i] = '\0';
	}
}

Token::Token(char* content) {
	name = new char[80];
	for (int i = 0; i < 80; i++) {
		name[i] = content[i];
	}
	if (name[0] == '<') {
		tokenType = 1;
		if (name[1] == '/') {
			++tokenType;
		}
	}
	else {
		tokenType = 0;
	}
}

Token::Token(char* content, int type) {
	name = new char[80];
	for (int i = 0; i < 80; i++) {
		name[i] = content[i];
	}
	tokenType = type;
}

int tokenize(Token* tokens, fstream &stream) {
	char x;
	//create the tokens
	int numTokens = 0;
	while (stream.get(x)) {
		char* token = new char[80];
		for (int i = 0; i < 80; i++) {
			token[i] = '\0';
		}
		if (x == '<') { //start or end tag
			token[0] = x;
			while (x != '>') {
				stream.get(x);
				strncat(token, &x, 1);
			}
			//add the token, and increment size
			tokens[numTokens++] = Token(token);
		}
		if (x > 47 && x < 58) { //is a number
			while (x > 47 && x < 58) {
				strncat(token, &x, 1);
				stream.get(x);
			}
			//add the token, and increment size
			tokens[numTokens++] = Token(token, 0);
			stream.unget(); //haha... what?
		}
	}
	return numTokens;
}

//compare a start and end tag, ignoring '<', '>', and '/'
bool tagsMatch(Token t, Token p) {
	//t is a start tag and p is an end tag
	if (t.tokenType == 1 && p.tokenType == 2) {
		for (int i = 0; i < 78; i++) {
			//check to see if each character of the names match
			//ignore the '<' of t and the '</' of p
			if (t.name[i + 1] != p.name[i + 2]) {
				return false;
			}
		}
		return true;
	}
	//t is an end tag and p is a start tag
	else if (t.tokenType == 2 && p.tokenType == 1) {
		for (int i = 0; i < 78; i++) {
			//check to see if each character of the names match
			//ignore the '</' of t and the '<' of p
			if (t.name[i + 2] != p.name[i + 1]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}

//true if the tokens form a valid xml structure, false otherwise
bool validateXML(Token* tokens, int numTokens) {
	stack<Token> tags = stack<Token>();
	//it must start with a start tag and end with an end tags
	if (tokens[0].tokenType != 1 || tokens[numTokens - 1].tokenType != 2) { 
		return false;
	}
	for (int i = 0; i < numTokens; i++) {
		if (tokens[i].tokenType == 1) {
			tags.push(tokens[i].name);
		}
		//if (tokens[i].tokenType == 0) //do nothing
		if (tokens[i].tokenType == 2) {
			Token p = tags.top();
			tags.pop();
			if (!tagsMatch(tokens[i], p)) {
				return false;
			}
		}
	}
	return true;
}

//Generate a generic tree from a list of tokens
//this doesnt validate the xml structure of the tokens
Element* createTree(Token* tokens, int numTokens) {
	stack<Element*> s = stack<Element*>();
	for (int i = 0; i < numTokens; i++) {
		if (tokens[i].tokenType == 1) {
			s.push(new Element(tokens[i].name));
		}
		if (tokens[i].tokenType == 0) {
			Element* e = s.top();
			s.pop();
			e->setValue(atoi(tokens[i].name));
			s.push(e);
		}
		if (tokens[i].tokenType == 2) {
			if (s.size() == 1) {
				return s.top();
			}
			Element* child = s.top();
			s.pop();
			Element* parent = s.top();
			s.pop();
			parent->addChild(child);
			s.push(parent);
		}
	}
	return s.top();//the root of the tree
}

int main(int argc, char* argv[]) {
	Token* tokens = new Token[300];
	Element* doc; // to store the data 
	stack<char*> tags; // for validation
	Element* result; // search result 
	fstream pfile; // file
	char keyword[256]; 
	char* filename;
	// check command line arguments 
	if( argc != 2) {
		cout << "Usage: " << argv[0] << " filename" << endl;
		return 0; 
	}
	filename = argv[1];
	// open the file
	pfile.open(filename, fstream::in);
	if (!pfile.is_open()) { // this was !pfile.open(), which is void...
		cout << "Unable to open the file: " << argv[1] << endl;
		return 0;
	}
	// read and process the input file (pfile)
	// 1. Split the file into tokens
	int numTokens = tokenize(tokens, pfile);
	// 2. Validate if the file is well-formed using tags
	bool valid = validateXML(tokens, numTokens);
	if (!valid) cout << "INVALID" << endl;
	// 3. Store the data in doc
	doc = createTree(tokens, numTokens);

	cout << "preorder traversal: \n";
	cout << *doc << endl; // print out the tree 
	cout << "The height of the tree is: " << doc->height() << endl; 
	cout << "The size of the tree is: " << doc->size() << endl;

	//---------------------------------------------------
	cout << "BONUS:\n";
	cout << "Demonstration of copy constructor: \n";
	Element* docCopy1 = new Element(*doc);
	docCopy1->setName("NotWeather");
	cout << "doc:\n";
	doc->display_xml();
	cout << "docCopy1:\n";
	docCopy1->display_xml();
	//---------------------------------------------------
	cout << "Demonstration of = operator: \n";
	Element docCopy2 = (*doc);
	docCopy2.setName("AlsoNotWeather");
	cout << "doc:\n";
	doc->display_xml();
	cout << "docCopy2:\n";
	docCopy2.display_xml();
	//---------------------------------------------------
	cout << "Demonstration of ChildFreeException:\n";
	try {
		Element* ElPaso = doc->find("ElPaso");
		list<Element*> missingChildren = ElPaso->getChildren().front()->getChildren();
	}
	catch (ChildFreeException) {
		cout << "ElPaso's child (temperature) doesnt have any children" << endl;
	}
	//---------------------------------------------------

	// search the tree 
	while(1)
	{
		cout << "Input a keyword to search: " << endl; 
		cin >> keyword;
		if (!strcmp(keyword, "QUIT"))
			break;
		result = doc->find(keyword); 
		if (result != NULL)
			result->display_xml(); 
		else
			cout << "Not found!" << endl;
	}

	//delete the documents
	delete doc;
	delete docCopy1;
	// close the file 
	pfile.close();
	return 0;
}