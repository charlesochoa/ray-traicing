#pragma once

#include <vector>
#include <string>

using namespace std;

class PNG
{
private:
	int	w,h;
	vector<float> px;

public:
	PNG(int width, int height);
	PNG(const string& name);

	void load(const string& name);
	void save(const string& name) const;
	int width() const { return w; }
	int height() const { return h; }
	int size() const { return w*h; }
	const vector<float>& pixels() const { return px; };
	void set(int i, int j, float r, float g, float b);
};
