#include <db/HashEquiJoin.h>
#include <unordered_map>
#include <vector>
#include <optional>
#include "db/IntField.h"
#include "db/StringField.h"

using namespace db;

HashEquiJoin::HashEquiJoin(JoinPredicate p, DbIterator *child1, DbIterator *child2)
        : predicate(p), child1(child1), child2(child2), currentBucketIterator(bucket.end()) {}

JoinPredicate *HashEquiJoin::getJoinPredicate() {
    return &predicate;
}

const TupleDesc &HashEquiJoin::getTupleDesc() const {
    return TupleDesc::merge(child1->getTupleDesc(), child2->getTupleDesc());
}

std::string HashEquiJoin::getJoinField1Name() {
    return child1->getTupleDesc().getFieldName(predicate.getField1());
}

std::string HashEquiJoin::getJoinField2Name() {
    return child2->getTupleDesc().getFieldName(predicate.getField2());
}

void HashEquiJoin::open() {
    child1->open();
    child2->open();
    buildHashTable();
    Operator::open();
}

void HashEquiJoin::close() {
    child1->close();
    child2->close();
    hashTable.clear();
    Operator::close();
}

void HashEquiJoin::rewind() {
    child1->rewind();
    child2->rewind();
    buildHashTable();
}

std::vector<DbIterator *> HashEquiJoin::getChildren() {
    return {child1, child2};
}

void HashEquiJoin::setChildren(std::vector<DbIterator *> children) {
    child1 = children[0];
    child2 = children[1];
}

std::optional<Tuple> HashEquiJoin::fetchNext() {
    while (true) {
        if (currentBucketIterator != bucket.end()) {
            Tuple tuple2 = child2->next();
            if (predicate.filter(&(*currentBucketIterator), &tuple2)) {
                Tuple t1 = *currentBucketIterator;
                Tuple t2 = tuple2;
                TupleDesc td = TupleDesc::merge(t1.getTupleDesc(), t2.getTupleDesc());
                Tuple resultTuple(td, nullptr);
                int i = 0;

                for (const auto &field : t1) {
                    resultTuple.setField(i, field);
                    i++;
                }

                for (const auto &field : t2) {
                    resultTuple.setField(i, field);
                    i++;
                }

                return resultTuple;
            }
            currentBucketIterator++;
        } else {
            if (child2->hasNext()) {
                Tuple tuple2 = child2->next();
                auto hashValue = hash(tuple2.getField(predicate.getField2()));
                auto it = hashTable.find(hashValue);
                if (it != hashTable.end()) {
                    bucket = it->second;
                    currentBucketIterator = bucket.begin();
                }
            } else {
                return std::nullopt;
            }
        }
    }
}

void HashEquiJoin::buildHashTable() {
    hashTable.clear();
    while (child1->hasNext()) {
        Tuple tuple = child1->next();
        auto hashValue = hash(tuple.getField(predicate.getField1()));
        hashTable[hashValue].push_back(tuple);
    }
}



size_t HashEquiJoin::hash(const Field &field) {
    switch (field.getType()) {
        case Types::INT_TYPE: {
            IntField u = (IntField &&) field;
            int intValue =u.getValue();  // Assuming this is the method to get integer value
            return std::hash<int>{}(intValue);
        }
        case Types::STRING_TYPE: {
            StringField u = (StringField &&) field;
            std::string stringValue = u.getValue();  // Assuming this is the method to get string value
            return std::hash<std::string>{}(stringValue);
        }
        default:
            // Handle unrecognized types or throw an exception
            throw std::runtime_error("Unsupported field type for hashing");
            // Or return a default hash value
            // return std::hash<int>{}(0);
    }
}
