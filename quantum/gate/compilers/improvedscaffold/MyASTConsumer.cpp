/*
 * MyASTConsumer.cpp
 *
 *  Created on: Apr 19, 2017
 *      Author: aqw
 */

#include "MyASTConsumer.hpp"

MyASTConsumer::MyASTConsumer() :
		Visitor(std::make_shared<MyASTVisitor>()) {
}

bool MyASTConsumer::HandleTopLevelDecl(DeclGroupRef DR) {
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
		// Traverse the declaration using our AST visitor.
		Visitor->TraverseDecl(*b);
	}
	return true;
}

