#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <utility>

using namespace std;
using namespace filesystem;

struct file_result
{
	bool success;
	int type;
};

/**
 * Checks if a file exists at the given file path and returns information about it.
 *
 * @param file_path The path to the file to check.
 * @return A file_result struct containing information about the file.
 */
file_result check_file(char *file_path)
{
	struct stat s;
	if (stat(file_path, &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return file_result({1, 0});
		else if (s.st_mode & S_IFREG)
			return file_result({1, 1});
		else
			return file_result({0, -1});
	}
	else
	{
		return file_result({0, -1});
	}
}

/**
 * Searches for a given string in a file.
 *
 * @param file_path The path to the file to search in.
 * @param search_string The string to search for.
 * @return true if the string was found in the file, false otherwise.
 */
bool search_file(string file_path, string search_string)
{
	ifstream file(file_path);
	bool success = 0;
	if (file.is_open())
	{
		string line;
		int line_count = 1;
		int col = -1;
		while (getline(file, line))
		{
			col = line.find(search_string);
			if (col != string::npos)
			{
				cout << "Found \"" << search_string << "\" at " << file_path << ":"
					 << line_count << ":" << col + 1 << endl;
				success = true;
			}
			line_count++;
		}
		file.close();
	}
	return success;
}

/**
 * Recursively searches a directory for a file containing a given search string.
 *
 * @param file_path The path of the directory to search.
 * @param search_string The string to search for in the files.
 * @return true if a file containing the search string is found, false otherwise.
 */
bool search_dir(string file_path, string search_string, int depth, int max_depth)
{
	string sub_path;
	file_result sub_result;
	bool success = 0;
	for (const auto &entry : directory_iterator(file_path))
	{
		sub_path = entry.path();
		sub_result = check_file((char *)sub_path.c_str());
		if (!sub_result.success)
			continue;
		if (sub_result.type == 0 && depth < max_depth)
			success = search_dir(sub_path, search_string, depth + 1, max_depth) || success;
		else if (sub_result.type == 1)
			success = search_file(sub_path, search_string) || success;
	}
	return success;
}

/**
 * Main function that searches for a given string in a file or directory.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return 0 if the search string is found, -1 otherwise.
 */
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "Usage: search file_path search_string [max_depth]" << endl;
		return -1;
	}

	file_result file_type = check_file(argv[1]);
	if (!file_type.success)
	{
		cout << "Could not search \"" << argv[1] << "\"" << endl;
		return -1;
	}

	int max_depth = 2;
	if (argc == 4)
	{
		max_depth = atoi(argv[3]);
		if (max_depth < 0)
		{
			cout << "Invalid max_depth" << endl;
			return -1;
		}
	}

	int search_result = 0;
	if (file_type.type == 0)
		search_result = search_dir(argv[1], argv[2], 0, max_depth);
	else if (file_type.type == 1)
		search_result = search_file(argv[1], argv[2]);

	if (!search_result)
	{
		cout << "\"" << argv[2] << "\""
			 << " not found" << endl;
		return -1;
	}
}
