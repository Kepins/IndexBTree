#ifndef DISKPAGE_H
#define DISKPAGE_H

#include <cstdint>

// memcpy()
#include <cstring>

class DiskPage {
	// Number of bytes in the page
	size_t size;
	// Is it dirty
	bool dirty;
	// Content of page
	char* content;
public:
	// Default constructor
	DiskPage();
	// Constructor
	DiskPage(size_t size);
	// Copy constructor
	DiskPage(const DiskPage& src);
	// Move constructor
	DiskPage(DiskPage&& t);
	// Assign operator
	DiskPage& operator=(const DiskPage& oth);
	// Assign move operator
	DiskPage& operator=(DiskPage&& t);
	// Destructor
	~DiskPage();


	// Return true is page has been modified
	bool isDirty() const { return dirty; }
	// Reset dirty
	void resetDirty() { this->dirty = false; }

	// Returns page content
	const char* getPageContent() const { return content; }
	// Sets page content
	void copyPageContent(const char* newContent, size_t newSize);
	// Moves page content
	void movePageContent(char* newContent, size_t newSize);

	// Get size of content
	size_t getSize() const { return size; }
};

#endif
