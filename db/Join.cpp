#include <db/Join.h>
#include <iostream>

using namespace db;

Join::Join(JoinPredicate *p, DbIterator *child1, DbIterator *child2) : predicate(p), child1(child1), child2(child2) {}

JoinPredicate *Join::getJoinPredicate() {
    // TODO pa3.1: some code goes here
    return this->predicate;
}

std::string Join::getJoinField1Name() {
    // TODO pa3.1: some code goes here
    return this->child1->getTupleDesc().getFieldName(predicate->getField1());
}

std::string Join::getJoinField2Name() {
    // TODO pa3.1: some code goes here
    return this->child2->getTupleDesc().getFieldName(predicate->getField2());
}

const TupleDesc &Join::getTupleDesc() const {
    // TODO pa3.1: some code goes here
    return TupleDesc::merge(child1->getTupleDesc(),child2->getTupleDesc());
}

void Join::open() {
    // TODO pa3.1: some code goes here
    this->child1->open();
    this->child2->open();
    Operator::open();
}

void Join::close() {
    // TODO pa3.1: some code goes here
    this->child1->close();
    this->child2->close();
    Operator::close();
}

void Join::rewind() {
    // TODO pa3.1: some code goes here
    this->child1->rewind();
    this->child2->rewind();
}

std::vector<DbIterator *> Join::getChildren() {
    // TODO pa3.1: some code goes here
    return {this->child1, this->child2};
}

void Join::setChildren(std::vector<DbIterator *> children) {
    // TODO pa3.1: some code goes here
    this->child1 = children[0];
    this->child2 = children[1];
}

std::optional<Tuple> Join::fetchNext() {
    // TODO pa3.1: some code goes here

    while (child1->hasNext() || currentTuple1.has_value()) {
        // Fetch the next tuple from child1 if there is no current tuple from child1
        if (!currentTuple1.has_value()) {
            currentTuple1 = child1->next();
            child2->rewind();
        }

        // Check tuples from child2 against the current tuple from child1
        while (child2->hasNext()) {
            Tuple tuple2 = child2->next();
            Tuple tuple1 = *currentTuple1;
           // std::cout<<tuple1.to_string()+" "+tuple2.to_string()<<std::endl;
            if (predicate->filter(&tuple1, &tuple2)) {
                // Concatenate the tuples and return the result
                TupleDesc td = TupleDesc::merge(child1->getTupleDesc(),child2->getTupleDesc());
                Tuple resultTuple(td, nullptr);
                int i=0;
                for (auto it : tuple1) {
                    // Process each field
                    // 'field' represents each element in the Tuple
                    resultTuple.setField(i, it);
                    i++;
                }

                for (auto& field : tuple2) {
                    // Process each field
                    // 'field' represents each element in the Tuple
                    resultTuple.setField(i, field);
                    i++;
                }
                return resultTuple;
            }
        }
        child2->rewind(); // Rewind child2 to start comparing with the new tuple from child1
        currentTuple1.reset();

    }
    child1->rewind();
    return std::nullopt; // No more tuples in child1

}
