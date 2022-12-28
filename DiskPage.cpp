#include "DiskPage.h"

DiskPage::DiskPage()
	:
	size(0)
{
	this->content = nullptr;

	this->dirty = false;
}

DiskPage::DiskPage(size_t size)
	:
	size(size)
{
	this->content = new char[size];

	this->dirty = false;
}

DiskPage::DiskPage(const DiskPage& src)
{
	if (this->content == nullptr) {
		this->content = new char[src.getSize()];
	}
	else if (this->getSize() != src.getSize()) {
		delete[] this->content;
		this->content = new char[src.getSize()];
	}
	memcpy(this->content, src.content, src.getSize());
	this->size = src.size;
	this->dirty = src.dirty;
}

DiskPage::DiskPage(DiskPage&& t)
{
	if (this->content != nullptr) {
		delete[] this->content;
	}
	this->content = t.content;
	this->size = t.size;
	this->dirty = t.dirty;
	t.size = 0;
	t.content = nullptr;
}

DiskPage& DiskPage::operator=(const DiskPage& oth)
{
	if (this->content == nullptr) {
		this->content = new char[oth.getSize()];
	}
	else if (this->getSize() != oth.getSize()) {
		delete[] this->content;
		this->content = new char[oth.getSize()];
	}
	memcpy(this->content, oth.content, oth.getSize());
	this->size = oth.size;
	this->dirty = oth.dirty;
	return *this;
}

DiskPage& DiskPage::operator=(DiskPage&& t)
{
	if (this->content != nullptr) {
		delete[] this->content;
	}
	this->content = t.content;
	this->size = t.size;
	this->dirty = t.dirty;
	t.size = 0;
	t.content = nullptr;

	return *this;
}

DiskPage::~DiskPage()
{
	if (this->content != nullptr) {
		delete[] content;
	}
}

void DiskPage::copyPageContent(const char* newContent, size_t newSize)
{
	if (content == nullptr) {
		content = new char[newSize];
	}
	else if(size != newSize) {
		delete[] content;
		content = new char[newSize];
	}
	size = newSize;
	memcpy(content, newContent, newSize);

	dirty = true;
}

void DiskPage::movePageContent(char* newContent, size_t newSize)
{
	if (content != nullptr) {
		delete[] content;
	}
	size = newSize;
	content = newContent;

	dirty = true;
}
