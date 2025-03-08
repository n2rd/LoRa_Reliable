/*
    OrifinL Author: Einar Arnason
    email: einsiarna@gmail.com

    A lightweight linked list type queue implementation,
    meant for microcontrollers.

    Usage and further info:
    https://github.com/EinarArnason/ArduinoQueue

    modified by Keith Robison N2SGL to take a priority value and insert based on the priority
    and pull it out with lower values
*/

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(ARDUINO) && ARDUINO < 100
#include "WProgram.h"
#endif

#if __cplusplus <= 199711L
#define nullptr NULL
#endif
//
// Based on ArduinoQueue, see above
//
template <typename T>
class ReversePriorityQueue {
  class Node {
   public:
    T item;
    Node* next;
    uint64_t priority;
    Node() { next = nullptr; }
    ~Node() { next = nullptr; }
  };

  Node* head;
  Node* tail;
  unsigned int maxItems;
  unsigned int maxMemory;
  unsigned int count;

 public:
 ReversePriorityQueue(unsigned int maxItems = (unsigned int)-1,
               unsigned int maxMemory = (unsigned int)-1) {
    this->head = nullptr;
    this->tail = nullptr;
    this->count = 0;
    this->maxMemory = maxMemory;
    this->maxItems = maxMemory / sizeof(Node);

    if (maxItems != 0 && this->maxItems > maxItems) {
      this->maxItems = maxItems;
    }
  }

  ~ReversePriorityQueue() {
    for (Node* node = head; node != nullptr; node = head) {
      head = node->next;
      delete node;
    }
  }

  /*
    Push an item to the queue.
    Returns false if memory is
    full, or true if the item
    was added to queue.
  */
  bool enqueue(T item, uint64_t priority) {
    if (count == maxItems) {
      return false;
    }

    Node* node = new Node;
    if (node == nullptr) {
      return false;
    }

    node->item = item;
    node->priority = priority;

    if (head == nullptr) {
      head = node;
      tail = node;
      count++;
      //log_d("initial head %d", (int)node->item);
      return true;
    }

    Node* comparee = head;
    Node* prevComparee = NULL;
    if (comparee->priority >= node->priority) {
        //Replace head 
        //log_d("replacing head %d", (int)node->item);
        node->next = head;
        head = node;
    } else {
        do {
            //climb up the nodes till our priority is between 2 nodes or equal to one
            prevComparee = comparee;
            comparee = comparee->next;
            if (comparee == NULL)
                break;
        } while (node->priority >= comparee->priority);
        if (comparee == NULL) {
            //Put at end of list like a regular FIFO
            //log_d("putting %d at end",(int)node->item);
            tail->next = node;
            tail = node;
        }
        else {
            //put at current node
            //log_d("Putting %d between %d and %d",(int)node->item, (int)prevComparee->item, (int)comparee->item);
            prevComparee->next = node;
            node->next = comparee;
        }
    }
    count++;

    return true;
  }

  /*
    Pop the front of the queue.
    Because exceptions are not
    usually implemented for
    microcontrollers, if queue
    is empty, a dummy item is
    returned.
  */
  T dequeue() {
    if ((count == 0) || (head == nullptr)) {
      return T();
    }

    Node* node = head;
    head = node->next;
    T item = node->item;
    delete node;
    node = nullptr;

    if (head == nullptr) {
      tail = nullptr;
    }

    count--;
    return item;
  }

  /*
    Returns true if the queue
    is empty, false otherwise.
  */
  bool isEmpty() { return head == nullptr; }

  /*
    Returns true if the queue
    is full, false otherwise.
  */
  bool isFull() { return count == maxItems; }

  /*
    Returns the number of items
    currently in the queue.
  */
  unsigned int itemCount() { return count; }

  /*
    Returns the size of the
    queue item in bytes.
  */
  unsigned int itemSize() { return sizeof(Node); }

  /*
    Returns the size of the queue
    (maximum number of items)
  */
  unsigned int maxQueueSize() { return maxItems; }

  /*
    Returns the size of the queue
    (maximum size in bytes)
  */
  unsigned int maxMemorySize() { return maxMemory; }

  /*
    Get the item in the front
    of the queue.
    Because exceptions are not
    usually implemented for
    microcontrollers, if queue
    is empty, a dummy item is
    returned.
  */
  T getHead() {
    if ((count == 0) || (head == nullptr)) {
      return T();
    }

    T item = head->item;
    return item;
  }

  /*
    Get the item in the back
    of the queue.
    Because exceptions are not
    usually implemented for
    microcontrollers, if queue
    is empty, a dummy item is
    returned.
  */
  T getTail() {
    if ((count == 0) || (head == nullptr)) {
      return T();
    }

    T item = tail->item;
    return item;
  }

  T* getHeadPtr() {
    if ((count == 0) || (head == nullptr)) {
      return nullptr;
    }

    return &(head->item);
  }

  T* getTailPtr() {
    if ((count == 0) || (head == nullptr)) {
      return nullptr;
    }

    return &(tail->item);
  }

  /*
    Depricated functions
  */

  // Depricated, use getHead() instead
  T front() { return getHead(); }
  // Depricated, use itemCount() instead
  unsigned int item_count() { return itemCount(); }
  // Depricated, use itemSize() instead
  unsigned int item_size() { return itemSize(); }
  // Depricated, use maxQueueSize() instead
  unsigned int max_queue_size() { return maxQueueSize(); }
  // Depricated, use maxMemorySize() instead
  unsigned int max_memory_size() { return maxMemorySize(); }
};
