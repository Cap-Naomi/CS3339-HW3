#include<iostream>
#include<fstream>

using namespace std;

class Entry {
public:
  Entry(){
    valid = false;
    tag = 0;
    ref = 0;
  }
  ~Entry();
  void display(ofstream& outfile){
    outfile << valid << " " << tag << " " << ref;
  }

  void set_tag(int _tag) { tag = _tag; }
  int get_tag() { return tag; }

  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }

  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

class Cache {
public:
  Cache(int _num_entries, int _assoc){
    num_entries = _num_entries;
    assoc = _assoc;
    num_sets = num_entries / assoc;
    timer = 0; // -----

    entries = new Entry*[assoc];

    for (int way = 0; way < assoc; way++) {
        entries[way] = new Entry[num_sets];
    }
  }
  ~Cache(){
    for (int way = 0; way < assoc; way++) {
        delete[] entries[way];
    }

    delete[] entries;
  }

  void display(ofstream& outfile){
    for (int set = 0; set < num_sets; set++) {
    outfile << "Set " << set << ": ";

    for (int way = 0; way < assoc; way++) {
        outfile << "[";
        entries[way][set].display(outfile);
        outfile << "] ";
    }

    outfile << endl;
    } 
  }

  int get_index(unsigned long addr) { return addr % num_sets; }
  int get_tag(unsigned long addr) { return addr / num_sets; }

  unsigned long retrieve_addr(int way, int index){
    return entries[way][index].get_tag() * num_sets + index;
  }
  
  bool hit(ofstream& outfile, unsigned long addr){
        int index = get_index(addr);
    int tag = get_tag(addr);

    for (int way = 0; way < assoc; way++) {
      if (entries[way][index].get_valid() &&
          entries[way][index].get_tag() == tag) {

          outfile << addr << " : HIT" << endl;

          entries[way][index].set_ref(timer++);
          return true;
      }
    }

    outfile << addr << " : MISS" << endl;
    return false;
  }

  void update(ofstream& outfile, unsigned long addr){
    int index = get_index(addr);
    int tag = get_tag(addr);

    /* First try empty slot */
    for (int way = 0; way < assoc; way++) {
        if (!entries[way][index].get_valid()) {
            entries[way][index].set_valid(true);
            entries[way][index].set_tag(tag);
            entries[way][index].set_ref(timer++);
            return;
        }
    }

    /* Replace LRU (smallest ref) */
    int victim = 0;
    int min_ref = entries[0][index].get_ref();

    for (int way = 1; way < assoc; way++) {
        if (entries[way][index].get_ref() < min_ref) {
            min_ref = entries[way][index].get_ref();
            victim = way;
        }
    }

    entries[victim][index].set_valid(true);
    entries[victim][index].set_tag(tag);
    entries[victim][index].set_ref(timer++);
}

  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;

  int timer;
};

