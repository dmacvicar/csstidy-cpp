/*
 * This file is part of CSSTidy.
 *
 * CSSTidy is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CSSTidy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CSSTidy; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

using namespace std;

#include "csspp_globals.hpp"

#include "prepare.hpp"

extern vector<string> raw_template;
extern string css_level;
extern map<string, vector<string> > predefined_templates;

int main(int argc, char *argv[])
{
	prepare();
	csstidy csst;

	if(argc > 1)
	{
		string filein = argv[1];
		if(filein != "-" && !file_exists(argv[1]))
		{
			cout << "The file \"" << filein << "\" does not exist." << endl;
			return EXIT_FAILURE;
		}
		
		string output_filename;
		
		for(int i = 2; i < argc; ++i)
		{
			bool output_file = true;
			for(map<string,int>::iterator j = csst.settings.begin(); j != csst.settings.end(); ++j )
			{
				if(trim(argv[i]) == "--" + j->first + "=false")
				{
					csst.settings[j->first] = 0;
					output_file = false;
				}
				else if(trim(argv[i]) == "--" + j->first + "=true")
				{
					csst.settings[j->first] = 1;
					output_file = false;
				}
			}
			if(trim(argv[i]).substr(0,12) == "--css_level=")
			{
				css_level = strtoupper(trim(argv[i]).substr(12));
				output_file = false;
			}
			else if(trim(argv[i]).substr(0,11) == "--template=")
			{
				string template_value = trim(argv[i]).substr(11);
				if(template_value == "high_compression" || template_value == "default" || template_value == "highest_compression" || template_value == "low_compression")
				{
					raw_template = predefined_templates[template_value];
				}
				else
				{
					string tpl_content = file_get_contents(template_value);
					if(tpl_content != "")
					{
						vector<string> tpl_arr = explode("|",tpl_content,true);
						if(tpl_arr.size() == 13)
						{
							raw_template = tpl_arr;
						}
					}
				}
				output_file = false;
			}
			if(output_file)
			{
				output_filename = trim(argv[i]);
			}
		}
		
		if(!csst.settings["allow_html_in_templates"])
		{
			for(int i = 0; i < raw_template.size(); ++i)
			{
				raw_template[i] = strip_tags(raw_template[i]);
			}
		}
		
		string css_file;
        if(filein == "-") {
			string temp;
			do {
				getline(cin, temp, '\n');
				css_file += (temp + "\n");
			} while(cin);
		} else {
            css_file = file_get_contents(argv[1]);
        }

		csst.parse_css(css_file);
		
		// Print CSS to screen if no output file is specified
		if(output_filename == "")
		{
			for(int i = 0; i < csst.csstokens.size(); i++)
			{
				cout << "type:" << csst.csstokens[i].type << ";data:" << csst.csstokens[i].data << endl;
			}
			csst.print_css();
		}
		else
		{
			csst.print_css(output_filename);
		}
		
		return EXIT_SUCCESS;
	}

	cout << endl << "Usage:" << endl << endl << "csstidy input_filename [\n";
	for(map<string,int>::iterator j = csst.settings.begin(); j != csst.settings.end(); ++j )
	{
		cout << " --" << j->first;
		if(j->second == true)
		{
			cout << "=[true|false] |\n";
		}
		else
		{
			cout << "=[false|true] |\n";
		}
	}
	cout << " --template=[default|filename|low_compression|high_compression|highest_compression] |\n";
	cout << " output_filename ]*" << endl;
	
	return EXIT_SUCCESS;
}
