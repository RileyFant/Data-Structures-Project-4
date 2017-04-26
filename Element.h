#ifndef ELEMENT_H
#define ELEMENT_H

#include <iostream>
#include <list>
#include <algorithm>

using namespace std;

class ElementException : exception { };
class ChildFreeException : ElementException { };
class ElementNotFoundException : ElementException { };

int numCalls = 0;

class Element {
private:
	list<Element*> children; 
	char* _tag;
	int _value;
	// private methods
public:
	// default constructor
	Element();
	// non-default constructors
	Element(char* tag); // value is set to -99 if not given 
	Element(char* tag, char* value);
	// destructor, must recursively destruct its children
	// and release the memory allocated for _tag
	~Element();
	// ostream operator ( pre-order traversal)
	friend ostream& operator << (ostream &out, const Element &E);
	void display_xml(); // print out the tree in xml-- format 
	void addChild( Element* child); // add a child
	// Find the first element such that _tag == tag
	// returns “this” pointer of this element
	Element* find(char* tag);
	char* getName() const;
	int getValue()const ;
	void setName(char* name);
	void setValue(int value);
	int height(); // return the height
	int size(); // return the size
	// other methods
	list<Element*> getChildren() const;

	/*BONUS*/
	Element(Element& e); //copy constructor
	void operator=(const Element& e); //overloaded = operator
};

//Default constructor
Element::Element() {
	_value = -99;
}

//Constructor with only tag given, _value set to -99
Element::Element(char* tag) {
	//find the length of the name
	int length = 0;
	while (tag[length] != '\0') {
		length++;
	}
	//copy name over to _tag
	_tag = new char[length+1];
	for (int i = 0; i < length + 1; i++) { 
		_tag[i] = '\0'; //fill with null characters
	}
	for (int j = 0; j < length; j++) { //leave the null-terminating character alone
		_tag[j] = tag[j+1]; //copy everything over, except '<'
	}
	_tag[length-2] = '\0'; //remove '>'
	_value = -99;
}

//Full constructor
Element::Element(char* tag, char* value) {
	//find the length of the name
	int length = 0;
	while (tag[length] != '\0') {
		length++;
	}
	//copy name over to _tag
	_tag = new char[length + 1];
	for (int i = 0; i < length + 1; i++) {
		_tag[i] = '\0'; //fill with null characters
	}
	for (int j = 0; j < length; j++) { //leave the null-terminating character alone
		_tag[j] = tag[j]; //copy everything over
	}

	//copy over value to _value
	_value = atoi(value);
}

//copy constructor
Element::Element(Element& e) {
	//copy over the _tag
	int length = 0;
	while (e.getName()[length] != '\0') {
		length++;
	}
	_tag = new char[length+1];
	for (int i = 0; i < length+1; i++) {
		_tag[i] = '\0'; //fill with null characters
	}
	for (int i = 0; i < length+1; i++) { //leave the null-terminating character alone
		_tag[i] = e.getName()[i]; //copy everything over, except '<'
	}

	//copy over the value
	_value = e.getValue();

	//copy over the children
	try {
		list<Element*> l = list<Element*>(e.getChildren());
		while (!l.empty()) {
			children.push_back(new Element((*l.front()))); //recursive
			l.pop_front();
		}
	}
	catch (ChildFreeException) {}
}

//Destructor, recursively destructs its children
Element::~Element() {
	// destructor, must recursively destruct its children
	list<Element*>::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		delete ((Element*)((void*)(*it)));
	}
	// and release the memory allocated for _tag
	delete[] _tag;
}

//overloaded = operator
void Element::operator=(const Element& e) { 
	//copy over the _tag
	int length = 0;
	while (e.getName()[length] != '\0') {
		length++;
	}
	_tag = new char[length];
	for (int i = 0; i < length; i++) {
		_tag[i] = '\0'; //fill with null characters
	}
	for (int i = 0; i < length; i++) { //leave the null-terminating character alone
		_tag[i] = e.getName()[i]; //copy everything over, except '<'
	}

	//copy over the value
	_value = e.getValue();

	//copy over the children
	try {
		list<Element*> l = list<Element*>(e.getChildren());
		while (!l.empty()) {
			children.push_back(new Element((*l.front()))); //recursive
			l.pop_front();
		}
	}
	catch (ChildFreeException) {}
}

//Overloaded << operator for Element (preorder traversal)
ostream& operator << (ostream &out, const Element &E) {
	
	list<Element*>::iterator it;
	out << E.getName() << " : " << E.getValue() <<endl;
	try {
		list<Element*> children = E.getChildren();
		for (it = children.begin(); it != children.end(); it++) {
			Element* e = ((Element*)((void*)(*it)));
			out << *e;
		}
	}
	catch (ChildFreeException) {}
	return out;
}

//Displays the tree in XML-- format
void Element::display_xml() {

	if (numCalls == 0) {
		cout << _tag << endl;
	}
	if (children.size() > 0) {
		numCalls++;
	}
	list<Element*> x = list<Element*>(children);
	while (!x.empty()) {
		for (int i = 0; i < numCalls; i++) {
			cout << "  ";
		}
		cout << x.front()->getName();
		if (x.front()->getValue() != -99) {
			cout << ": " << x.front()->getValue();
		}
		cout << endl;
		x.front()->display_xml();
		x.pop_front();
		if (x.size() == 0) {
			numCalls--;
		}
	}
	
}

//add a child Element
void Element::addChild(Element* child) {
	children.push_back(child);
}

// Find the first element such that _tag == tag
// returns “this” pointer of this element
Element* Element::find(char* tag) {
	//the case when it's found
	if (strcmp(_tag, tag) == 0) {
		return this;
	}
	//look at each of this' children
	else {
		if (children.size() == 0) {
			return NULL;
		}
		for (list<Element*>::iterator i = children.begin(); i != children.end(); i++) {
			Element* temp = ((Element*)((void*)(*i)))->find(tag);
			if (temp != NULL) return temp;
		}
		return NULL;
	}
}

char* Element::getName() const {
	return _tag;
}

int Element::getValue() const {
	return _value;
}

void Element::setName(char* name) {
	//find the length of the name
	int length = 0;
	while (name[length] != '\0') {
		length++;
	}
	//copy name over to _tag
	_tag = new char[length + 1];
	for (int i = 0; i < length + 1; i++) {
		_tag[i] = '\0'; //fill with null characters
	}
	for (int j = 0; j < length; j++) { //leave the null-terminating character alone
		_tag[j] = name[j]; //copy everything over
	}
}

void Element::setValue(int value) {
	_value = value;
}

int Element::height() {
	if (children.size() == 0) {
		return 1;
	}
	else {
		list<Element*>::iterator it;
		int maxHeight = 0;

		for (it = children.begin(); it != children.end(); it++) {
			int temp = ((Element*)((void*)(*it)))->height() + 1;
			if (maxHeight < temp) {
				maxHeight = temp;
			}
		}
		return maxHeight;
	}
	/*if (children.size() > 0) {
		numCalls++;
	}
	if (numCalls > maxHeight) {
		maxHeight = numCalls;
	}
	list<Element*> x = list<Element*>(children);
	while (!x.empty()) {
		x.front()->height();
		x.pop_front();
		if (x.size() == 0) {
			numCalls--;
		}
	}
	return maxHeight + 1;*/
}

int Element::size() {
	//if this is a leaf, end recursion (base case)
	if (children.size() == 0) {
		return 1;
	}
	else {
		list<Element*>::iterator it;
		int sum = 1;
		for (it = children.begin(); it != children.end(); it++) {
			sum += ((Element*)((void*)(*it)))->size();
		}
		return sum;
	}
	//otherwise, return the sume of the sizes of each of the children
	
}

list<Element*> Element::getChildren() const {
	if (children.size() == 0) {
		throw ChildFreeException();
	}
	return children;
}

#endif