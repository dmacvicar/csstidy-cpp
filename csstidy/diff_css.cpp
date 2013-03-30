/*
 * Copyright 2013 Duncan Mac-Vicar <dmacvicar@suse.de>
 *
 * This file is part of CSSTidy.
 *
 *  CSSTidy is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   CSSTidy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "csspp_globals.hpp"

using namespace std;

csstidy csstidy::diff_css(csstidy prev)
{
    csstidy ret;
    ret.settings["preserve_css"] = true;

    // first look for styles that are in the prev
    // but not in the current one. Those need to be
    // disabled manually
    css_struct removed;
    for (css_struct::iterator i = prev.css.begin(); i != prev.css.end(); ++i)
    {
        if (settings["sort_selectors"]) i->second.sort();
        for(sstore::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
            for(umap<string,string>::iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
                if (! css[i->first][j->first].has(k->first)) {
                    removed[i->first][j->first][k->first] = k->second;
                }
            }
        }
    }

    for (css_struct::iterator i = css.begin(); i != css.end(); ++i)
    {
        if (settings["sort_selectors"]) i->second.sort();

        for(sstore::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
            ret.add_token(SEL_START, j->first, true);

            // add removed properties commented out until we get
            // a "undefine" algorithm
            if (removed[i->first].has(j->first))
            {
                for(umap<string,string>::iterator k = removed[i->first][j->first].begin(); k != removed[i->first][j->first].end(); ++k)
                {
                    ret.add_token(COMMENT, k->first + ":" + k->second, true);
                }
            }

            for(umap<string,string>::iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
                if (prev.css[i->first][j->first].has(k->first) &&
                    css[i->first][j->first][k->first] == prev.css[i->first][j->first][k->first])
                {
                    continue;
                }

                ret.add_token(PROPERTY, k->first, true);
                ret.add_token(VALUE, k->second, true);
            }
            ret.add_token(SEL_END, j->first, true);
        }
    }
    return ret;
}

