## Report on Programming Assignment 3: Database System Implementation

### Exercise 1: Filter and Join

### Implementation of Filter Operator:

This functionality is fundamental in SQL queries where WHERE clauses are used.

##### Code Structure:

Constructor: The Filter class is initialized with a Predicate and a child DbIterator. The Predicate object encapsulates the condition to be checked against each tuple.

getPredicate() Method: Returns a pointer to the predicate used for filtering.

getTupleDesc() Method: Delegates to the child operator to obtain the tuple description, which remains unchanged by the filter operation.

open(), close(), rewind() Methods: These control the state of the filter operator, opening, closing, and resetting the iteration of tuples, respectively.

getChildren() and setChildren() Methods: Used for getting and setting the child operators of this filter operator.

fetchNext() Method: This is where the core functionality resides. It iterates over the tuples from the child operator, applying the predicate to each. If a tuple satisfies the predicate, it is returned; otherwise, the iteration continues until no more tuples are available.

### Implementation of Join Operator

##### Code Structure

Constructor: Initializes the Join class with two child DbIterators and a JoinPredicate. The predicate defines the conditions for joining tuples from the two child iterators.

Join Predicate Methods: Includes getJoinPredicate(), getJoinField1Name(), and getJoinField2Name(), which provide access to the join predicate and the names of the join fields from both iterators.

getTupleDesc() Method: Generates a merged TupleDesc object that combines the tuple descriptions from both child iterators, reflecting the structure of the joined tuples.

State Management Methods: open(), close(), and rewind() control the state of the join operation, managing the opening, closing, and resetting of the iterators.

Children Management: Methods getChildren() and setChildren() are used for handling the child operators of the join.

fetchNext() Method: This method encapsulates the core logic of the join operation. It iteratively fetches tuples from the first child and then checks each tuple from the second child for a match based on the join predicate. When a matching pair is found, it concatenates these tuples and returns them. This process continues until all possible pairings have been evaluated.

### Implementation of JoinPredicate

##### Code Structure
Constructor: Initializes the JoinPredicate with field indices and an operator. The fields represent the indices in the tuples to be joined, and the operator (Predicate::Op) defines the type of comparison (e.g., equal, less than).

filter() Method: This method applies the predicate to the two specified tuples. It uses the compare method from the Field class to perform the actual comparison.

Accessor Methods: getField1(), getField2(), and getOperator() provide access to the private members of the class, allowing other components, especially the Join operator, to understand the nature of the predicate.

### Implementation of HashEquiJoin Operator

##### Code Structure

Constructor: Initializes HashEquiJoin with a JoinPredicate and two child DbIterators. It also initializes the iterator for the current bucket in the hash table.

Hash Table Building: The buildHashTable() method constructs a hash table for one of the relations (child1). This table maps hash values of the join attribute to the corresponding tuples.

Hashing Function: The hash() method creates hash values for a given field. It handles different field types like integers and strings.

Join Operation: The fetchNext() method implements the core join logic. It iterates through tuples in the second child, finds corresponding buckets in the hash table using the hash value of the join attribute, and then iterates through the bucket to find matching tuples based on the join predicate.

State Management and Children Handling: Similar to the Join operator, it has methods like open(), close(), rewind(), getChildren(), and setChildren() for managing the operator's state and its children.

### Exercise 2: Aggregate

### Implementation of Aggregate Operator

##### Code Structure
Constructor: Initializes the operator with a child DbIterator, aggregate field (afield), group field (gfield), and the aggregation operation (aop). The constructor also prepares the TupleDesc based on whether the aggregation involves grouping.

Aggregation Functionality: The fetchNext() method retrieves the next aggregate result. It uses an Aggregator (either IntegerAggregator or StringAggregator, depending on the field type) to compute the aggregate values.

Accessor Methods: Methods like groupField(), groupFieldName(), aggregateField(), and aggregateFieldName() provide access to the properties of the aggregate operation.

State Management: Methods open(), close(), and rewind() manage the opening, closing, and resetting of the operator and its child iterator.

Tuple Description: The getTupleDesc() method returns the TupleDesc for the output of the aggregate, which changes depending on whether there is grouping.


### Implementation of IntegerAggregator

##### Code Structure
Constructor: Sets up the aggregator with information about the group-by field (gbfield), the type of the group-by field (gbfieldtype), the aggregate field (afield), and the aggregation operation (what). It initializes internal structures based on whether grouping is required.

Aggregation Logic: The mergeTupleIntoGroup() method integrates new tuples into the aggregate calculation, grouping them as specified in the constructor.

Iterator Generation: The iterator() method creates an iterator over the group aggregate results, returning tuples that either consist of the group value and aggregate value (if grouping) or just the aggregate value.

Auxiliary Methods: Additional private methods like initialValue() and mergeValue() are used internally to initialize and update aggregate values.

### Exercise 3: Insertion and deletion
### Implementation of Insert Operator
##### Code Structure
Constructor: Sets up the operator with a transaction ID, the child operator from which to read tuples, and the table ID where tuples should be inserted.

Insert Operation: The fetchNext() method processes tuples from the child operator and inserts them into the specified table. It returns a tuple indicating the number of inserted records.

State Management: Similar to other operators, open(), close(), and rewind() methods manage the state of the operator and its interaction with the child operator.

### Implementation of Delete Operator
##### Code Structure
Constructor: Configured with a transaction ID and the child operator from which tuples will be read for deletion.

Delete Operation: The fetchNext() method reads tuples from the child operator and deletes them from their respective tables. It returns a tuple indicating the number of deleted records.

State Management: Implements open(), close(), and rewind() methods for managing the operator's state and its interaction with the child operator.

