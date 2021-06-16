/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *    Alexander J. McCaskey - initial API and implementation
 *    Daniel Claudino - porting
 ******************************************************************************/

#ifndef BK_FENWICK_HPP_
#define BK_FENWICK_HPP_

#include <vector>
#include <set>

/**
 * This class represents a Node in the Fenwick Tree. It keeps track
 * of its parent, integer index, and any children it has.
 */
class Node : public std::enable_shared_from_this<Node> {

public:
	using NodePtr = std::shared_ptr<Node>;

	/**
	 * Reference to this Node's parent Node
	 */
	NodePtr parent;

	/**
	 * Reference to this Node's children Nodes.
	 */
	std::set<NodePtr> children;

	/**
	 * Reference to this Node's index
	 */
	int index;

	/**
	 * The constructor
	 */
	Node() : index(-1), children(std::set<NodePtr>{}) {}

	/**
	 * Return all predecessor Nodes.
	 *
	 * @return ancestors This Node's predecessor Nodes
	 */
	std::set<NodePtr> getAncestors() {
		auto node = shared_from_this();
		std::set<NodePtr> ancestors;
		while (node->parent) {
			ancestors.insert(node->parent);
			node = node->parent;
		}
		return ancestors;
	}
};

/**
 * This class models a Fenwick Tree and provides a way to
 * map binary strings to binary strings such that both the
 * predecessor sum and any bit flip operations have O(logN) access cost.
 *
 * See https://arxiv.org/pdf/1701.07072.pdf
 */
class FenwickTree {

	using NodePtr = std::shared_ptr<Node>;

protected:

	/**
	 * Reference to the Nodes in this tree
	 */
	std::vector<NodePtr> nodes;

	/**
	 * The root node of this Tree.
	 */
	NodePtr root;

public:

	/**
	 * The Constructor.
	 *
	 * @param nQubits
	 */
	FenwickTree(const int nQubits) {

		 // Initialize with nQubits Nodes.
		for (int i = 0; i < nQubits; i++) {
			nodes.push_back(std::make_shared<Node>());
		}

		if (nQubits > 0 ) {

			// Set the Root node and its index
			root = nodes[nQubits-1];
			root->index = nQubits - 1;

			// Create a recursive lambda that will let
			// us build up the tree.
			std::function<void(const int, const int, NodePtr)> construct;
			construct =
					[this,&construct](const int idx1, const int idx2, NodePtr parent) {
						if( idx1 >= idx2) {
							return;
						} else {
							auto pvt = (idx1 + idx2) >> 1;
							auto c = nodes[pvt];

							c->index = pvt;
							parent->children.insert(c);
							c->parent = parent;

							construct(idx1, pvt, c);
							construct(pvt + 1, idx2, parent);
						}
					};

			// Construct the Tree!
			construct(0, nQubits-1, root);
		}
	}

	/**
	 * Return the update set corresponding to Node i.
	 *
	 * @param i
	 * @return
	 */
	std::set<NodePtr> getUpdateSet(const int i) {
		return nodes[i]->getAncestors();
	}

	/**
	 * Return the children of Node i.
	 *
	 * @param i
	 * @return
	 */
	std::set<NodePtr> getChildrenSet(const int i) {
		return nodes[i]->children;
	}

	/**
	 * Return the remainder set for Node i.
	 *
	 * @param i
	 * @return
	 */
	std::set<NodePtr> getRemainderSet(const int i) {
		std::set<NodePtr> rset;
		auto ancestors = getUpdateSet(i);

		for (auto a : ancestors) {
			for (auto c : a->children) {
				if (c->index < i) {
					rset.insert(c);
				}
			}
		}

		return rset;
	}

	/**
	 * Return the parity set for Node i.
	 *
	 * @param i
	 * @return
	 */
	std::set<NodePtr> getParitySet(const int i) {
		auto result = getRemainderSet(i);
		auto cs = getChildrenSet(i);

		std::set<NodePtr> paritySet = result;
		paritySet.insert(cs.begin(), cs.end());

		return paritySet;
	}
};


#endif /* VQE_TRANSFORMATION_BK_FENWICK_HPP_ */