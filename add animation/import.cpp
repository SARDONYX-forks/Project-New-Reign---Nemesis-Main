#include "import.h"

using namespace std;

vecstr importOutput(vector<ImportContainer> ExportID, int counter, int nextID, string file)
{
	vecstr behaviorlines;
	ImportContainer newExportID;
	vector<ImportContainer> exportID = ExportID;
	int lastID = nextID;

	for (auto it = exportID[counter].begin(); it != exportID[counter].end(); ++it)
	{
		string filename = "behavior templates\\" + it->first + ".txt";

		if (!isFileExist(filename))
		{
			cout << "ERROR(1027): Missing import file. Please contact the template creator" << endl << "File : " << filename << endl << endl;
			error = true;
			return behaviorlines;
		}

		vecstr exportFormat = GetFunctionLines(filename);

		if (error)
		{
			return behaviorlines;
		}

		behaviorlines.reserve(exportFormat.size());

		for (auto iter = it->second.begin(); iter != it->second.end(); ++iter)
		{
			if (iter->second.length() > 0)
			{
				bool norElement = false;
				int openRange = 0;
				int elementLine = 0;
				int eleCounter = 0;
				SSMap IDExist;
				behaviorlines.reserve(behaviorlines.size() + exportFormat.size() + 1);

				for (unsigned int j = 0; j < exportFormat.size(); j++)
				{
					bool elementCatch = false;
					string line = exportFormat[j];
					
					if (line.find("<hkparam name=\"") != string::npos && line.find("numelements=\"") != string::npos && line.find("</hkparam>") == string::npos && line.find("<!-- COMPUTE -->", line.find("numelements=\"")) != string::npos)
					{
						if (!norElement)
						{
							norElement = true;
							elementCatch = true;
							string templine = line.substr(0, line.find("<hkparam name=\"", 0));
							openRange = count(templine.begin(), templine.end(), '\t');
						}
						else
						{
							cout << "ERROR(1136): Multiple computation function running concurrently detected. Only 1 computation function can be done at a time" << endl << "Import File: " << filename << endl << "Line: " << j + 1 << endl << endl;
							error = true;
							behaviorlines.shrink_to_fit();
							return behaviorlines;
						}
					}
					else if (line.find("</hkparam>") != string::npos && norElement)
					{
						string templine = line.substr(0, line.find("</hkparam>"));
						__int64 range = count(templine.begin(), templine.end(), '\t');

						if (openRange == range)
						{
							string oldElement;

							if (exportFormat[elementLine].find("numelements=\"$elements$\">", 0) == string::npos)
							{
								size_t position = exportFormat[elementLine].find("numelements=\"") + 13;
								oldElement = exportFormat[elementLine].substr(position, exportFormat[elementLine].find("\">", position) - position);
							}
							else
							{
								oldElement = "$elements$";
							}

							if (oldElement != to_string(eleCounter))
							{
								exportFormat[elementLine].replace(exportFormat[elementLine].find(oldElement), oldElement.length(), to_string(eleCounter));
							}

							norElement = false;
							eleCounter = 0;
							elementLine = -1;
						}
					}

					if (norElement)
					{
						string templine = line;

						if (templine.find("<hkobject>") != string::npos)
						{
							templine = templine.substr(0, templine.find("<hkobject>"));
							__int64 range = count(templine.begin(), templine.end(), '\t');

							if (range == openRange + 1)
							{
								eleCounter++;
							}
						}
						else if (templine.find("\t\t\t#") != string::npos)
						{
							templine = templine.substr(0, templine.find("#", 0));
							__int64 reference = count(templine.begin(), templine.end(), '\t');

							if (reference == openRange + 1)
							{
								__int64 number = count(line.begin(), line.end(), '#');
								eleCounter += number;
							}
						}
					}

					if (line.find("$import[1][2]$", 0) != string::npos)
					{
						int reference = sameWordCount(line, "$import[1][2]$");

						for (int k = 0; k < reference; k++)
						{
							line.replace(line.find("$import[1][2]$"), 14, iter->second);
						}
					}

					if (line.find("$import[1]$", 0) != string::npos)
					{
						int reference = sameWordCount(line, "$import[1]$");

						for (int k = 0; k < reference; k++)
						{
							line.replace(line.find("$import[1]$"), 11, it->first);
						}
					}

					if (line.find("$import[", 0) != string::npos && line.find("]$", line.find("$import[" + 1)) != string::npos)
					{
						int reference = sameWordCount(line, "$import[");

						for (int k = 0; k < reference; k++)
						{
							string number = boost::regex_replace(string(line.substr(line.find("$import["))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));

							if (line.find("$import[" + number + "]$", 0) != string::npos)
							{
								// check if it is number
								for (size_t p = 0; p < number.size(); ++p)
								{
									if (!isalnum(number[p]) || isalpha(number[p]))
									{
										cout << "ERROR(1154): Invalid import input. Only number is acceptable. Please contact the template creator" << endl << "Template: " << it->first << endl << "Line: " << j + 1 << endl << endl;
										error = true;
										behaviorlines.shrink_to_fit();
										return behaviorlines;
									}
								}

								int num = stoi(number);
								stringstream sstream(iter->first);
								istream_iterator<string> ssbegin(sstream);
								istream_iterator<string> ssend;
								vecstr keywords(ssbegin, ssend);
								copy(keywords.begin(), keywords.end(), keywords.begin());

								if (num - 2 >= int(keywords.size()))
								{
									cout << "ERROR(1169): Missing import input. Please contact the template creator" << endl << "Template: " << it->first << endl << "Line: " << j + 1 << endl << endl;
									error = true;
									behaviorlines.shrink_to_fit();
									return behaviorlines;
								}

								line.replace(line.find("$import[" + number + "]$"), 10 + number.length(), keywords[num - 2]);
							}
						}
					}

					if (line.find("$import[", 0) != string::npos && line.find("]$", line.find("$import[" + 1)) != string::npos)
					{
						size_t nextpos = line.find("$import[");
						string importer = line.substr(nextpos + 1, line.find("]$") - nextpos);
						size_t bracketCount = count(importer.begin(), importer.end(), '[');
						size_t altBracketCount = count(importer.begin(), importer.end(), ']');

						if (IDExist[importer].length() == 0)
						{
							if (bracketCount != altBracketCount)
							{
								cout << "ERROR(1139): Invalid import element. Please contact the template creator" << endl << "Template: " << it->first << endl << "Line: " << j + 1 << endl << "Import: " << importer << endl << endl;
								error = true;
								behaviorlines.shrink_to_fit();
								return behaviorlines;
							}

							size_t pos = importer.find("[") + 1;
							string file = importer.substr(pos, importer.find("]", pos) - pos);
							string keyword;
							string tempID;

							if (bracketCount > 1)
							{
								pos = importer.find("[", pos) + 1;
								keyword = importer.substr(pos, importer.find("]", pos) - pos);
							}
							else
							{
								keyword = "";
							}

							for (unsigned int i = 0; i < exportID.size(); ++i)
							{
								if (exportID[i][file][keyword].length() > 0)
								{
									tempID = exportID[i][file][keyword];
									break;
								}
							}

							if (tempID.length() == 0)
							{
								tempID = to_string(lastID);

								for (unsigned int i = 0; i < 4 - tempID.length(); i++)
								{
									tempID = "0" + tempID;
								}

								IDExist[importer] = tempID;
								newExportID[file][keyword] = tempID;
								exportID.push_back(newExportID);
								lastID++;
							}

							line.replace(nextpos, importer.length() + 2, tempID);
						}
						else
						{
							line.replace(nextpos, importer.length() + 2, IDExist[importer]);
						}
					}

					if (line.find("MID$", 0) != string::npos)
					{
						int reference = sameWordCount(line, "MID$");

						for (int k = 0; k < reference; k++)
						{
							string tempID = line.substr(line.find("MID$"));
							string number = boost::regex_replace(string(tempID), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
							string oldID = "MID$" + number;

							if (line.find(oldID) != string::npos)
							{
								size_t nextpos = line.find(oldID);

								if (IDExist[oldID].length() > 0)
								{
									tempID = IDExist[oldID];
								}
								else
								{
									tempID = to_string(lastID);

									for (unsigned int i = 0; i < 4 - tempID.length(); i++)
									{
										tempID = "0" + tempID;
									}

									IDExist[oldID] = tempID;
									lastID++;
								}

								line.replace(nextpos, oldID.length(), tempID);
							}
							else
							{
								cout << "ERROR(1028): Invalid ID. Please contact the template creator" << endl << "File : " << iter->first << "Line: " << j + 1 << endl << endl;
								error = true;
								behaviorlines.shrink_to_fit();
								return behaviorlines;
							}
						}
					}

					if (elementCatch)
					{
						elementLine = behaviorlines.size();
					}

					behaviorlines.push_back(line);
				}

				if (behaviorlines.size() != 0 && behaviorlines.back().length() != 0)
				{
					behaviorlines.push_back("");
				}
			}
		}
	}

	if (newExportID.size() != 0)
	{
		vecstr additionlines = importOutput(exportID, int(exportID.size() - 1), lastID, file);
		behaviorlines.reserve(behaviorlines.size() + additionlines.size());
		behaviorlines.insert(behaviorlines.end(), additionlines.begin(), additionlines.end());
	}
	else
	{
		behaviorlines.shrink_to_fit();
	}

	return behaviorlines;
}