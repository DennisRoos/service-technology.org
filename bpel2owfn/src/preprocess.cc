#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;


/// the "known" elements
string keywords[] = { "assign", "case", "catch", "catchAll", "compensate", "compensationHandler", "copy", "correlation", "correlations", "correlationSet", "correlationSets", "empty", "eventHandlers", "faultHandlers", "flow", "from", "import", "invoke", "link", "links", "onAlarm", "onMessage", "otherwise", "partner", "partnerLink", "partnerLinks", "partners", "pick", "process", "receive", "reply", "scope", "sequence", "source", "switch", "target", "terminate", "throw", "to", "variable", "variables", "wait", "while", "?xml" };

/// the size of keywords[]
const unsigned int keywordCount = 44;


// some global variables
bool parsingBPEL = true;
bool parsingComments = false;
string closingTag;
set<string> ignoredTags;


/// converts a char to a C++ string
string toString(char &c)
{
  char* s = (char*)malloc(2 * sizeof(char));
  s[0] = c;
  s[1] = '\0';
  return string(s);
}


/// checks whether a given string matches with one of the known elements
bool isKeyword(string s)
{
  if (s == "!--")
  {
    parsingComments = true;
    return true;
  }

  for (int i = 0; i < keywordCount; i++)
    if (s == keywords[i] || s == ("/" + keywords[i]) || (s == "bpws:" + keywords[i]) || (s == "/bpws:" + keywords[i]))
      return true;

  ignoredTags.insert(s);
  return false;
}


string getNextWord(ifstream &inputStream)
{
  string result;

  char ch;
  while( inputStream.get(ch) )
  {
    if ( ch == '/' || ch == ' ' || ch == '>' || ch == '\n' || ch == '\t' || ch == '\r' )
    {
      if (result != "")
      {
	if (parsingBPEL)
	{
	  if (isKeyword(result))
	    return (result + toString(ch));
	  else
	  {
	    if (ch != '/')
	    {
	      parsingBPEL = false;
	      closingTag = result;
	      return ("!-- <" + result + toString(ch));
	    }
	    else
	      return ("!-- <" + result + "/>" + " --");
	  }
	}
	else
	{
  	  if (result == ("/" + closingTag))
  	  {
  	    parsingBPEL = true;
  	    return (result + toString(ch) + " -->");
  	  }
  	  return (result + toString(ch));
	}
      }
    }
    
    result += toString(ch);
  }

  /* should never reach this line */
}



int main(int argc, char *argv[])
{
  ifstream inputStream;
  inputStream.open(argv[1]);

  char ch;
  char old_ch;
  while( inputStream.get(ch) )
  {
    if (ch == '<' && !parsingComments)
      cout << "<" << getNextWord(inputStream);
    else
    {
      if (ch == '>' && old_ch == '-')
	parsingComments = false;
	
      cout << ch;
    }

    old_ch = ch;
  }

  cout << flush;
  inputStream.close();

  if (!ignoredTags.empty())
  {
    cerr << ignoredTags.size() << " ignored tags: " << endl;
    for (set<string>::iterator it = ignoredTags.begin(); it != ignoredTags.end(); it++)
      cerr << "  <" << *it << ">" << endl;
  }
}
