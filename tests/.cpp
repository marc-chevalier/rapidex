#include<vector>
#include<string>
#include<random>
#include<unordered_set>
#include "../include/FunkyVariableNameDispenser.h"

using namespace std;
string genTitle();
string color(const string& str);
string genRandomName();
string genRandomNameOrNumber();
string genRandomColor();

string FunkyVariableNameDispenser::genNewVariable(const set<string>& reserved)
{
    return genNewVariables(1, reserved)[0];
}

vector<string> FunkyVariableNameDispenser::genNewVariables(size_t nbr, const set<string>& reserved)
{
    vector<string> output;
    unordered_set<string> output_;
    random_device rd;
    for(size_t i=0; i<nbr;)
    {
        string name = color(genRandomName());

        random_device::result_type layout = rd()%5;
        switch(layout)
        {
            case 0:
                name = name+"^{"+color(genRandomNameOrNumber())+"}";
                break;
            case 1:
                name = name+"_{"+color(genRandomNameOrNumber())+"}";
                break;
            case 2:
                name = "\\overset{"+color(genRandomNameOrNumber())+"}{"+name+"}";
                break;
            case 3:
                name = "\\overset{"+color(genRandomNameOrNumber())+"}{"+name+"}";
                break;
            case 4:
                name = "\\sqrt["+color(genRandomNameOrNumber())+"]{"+name+"}";
                break;
            default:
                break;
        }

        if(reserved.count(name) == 0 && output_.count(name) == 0)
        {
            output_.insert(name);
            ++i;
        }
    }

    for(string name : output_)
        output.push_back(name);

    return output;
}

string color(const string& str)
{
    return "\\textcolor{"+genRandomColor()+"}{"+str+"}";
}

string genRandomNameOrNumber()
{
    random_device rd;
    random_device::result_type choice = rd()%2;

    if(choice)
        return to_string(rd()%100);
    return genRandomName();
}

string genRandomColor()
{
    random_device rd;
    vector<string> colors;
    colors.push_back("Apricot");
    colors.push_back("Aquamarine");
    colors.push_back("Bittersweet");
    colors.push_back("Black");
    colors.push_back("Blue");
    colors.push_back("BlueGreen");
    colors.push_back("BlueViolet");
    colors.push_back("BrickRed");
    colors.push_back("Brown");
    colors.push_back("BurntOrange");
    colors.push_back("CadetBlue");
    colors.push_back("CarnationPink");
    colors.push_back("Cerulean");
    colors.push_back("CornflowerBlue");
    colors.push_back("Cyan");
    colors.push_back("Dandelion");
    colors.push_back("DarkOrchid");
    colors.push_back("Emerald");
    colors.push_back("ForestGreen");
    colors.push_back("Fuchsia");
    colors.push_back("Goldenrod");
    colors.push_back("Gray");
    colors.push_back("Green");
    colors.push_back("GreenYellow");
    colors.push_back("JungleGreen");
    colors.push_back("Lavender");
    colors.push_back("LimeGreen");
    colors.push_back("Magenta");
    colors.push_back("Mahogany");
    colors.push_back("Maroon");
    colors.push_back("Melon");
    colors.push_back("MidnightBlue");
    colors.push_back("Mulberry");
    colors.push_back("NavyBlue");
    colors.push_back("OliveGreen");
    colors.push_back("Orange");
    colors.push_back("OrangeRed");
    colors.push_back("Orchid");
    colors.push_back("Peach");
    colors.push_back("Periwinkle");
    colors.push_back("PineGreen");
    colors.push_back("Plum");
    colors.push_back("ProcessBlue");
    colors.push_back("Purple");
    colors.push_back("RawSienna");
    colors.push_back("Red");
    colors.push_back("RedOrange");
    colors.push_back("RedViolet");
    colors.push_back("Rhodamine");
    colors.push_back("RoyalBlue");
    colors.push_back("RoyalPurple");
    colors.push_back("RubineRed");
    colors.push_back("Salmon");
    colors.push_back("SeaGreen");
    colors.push_back("Sepia");
    colors.push_back("SkyBlue");
    colors.push_back("SpringGreen");
    colors.push_back("Tan");
    colors.push_back("TealBlue");
    colors.push_back("Thistle");
    colors.push_back("Turquoise");
    colors.push_back("Violet");
    colors.push_back("VioletRed");
    colors.push_back("White");
    colors.push_back("WildStrawberry");
    colors.push_back("Yellow");
    colors.push_back("YellowGreen");
    colors.push_back("YellowOrange");

    return colors[rd()%colors.size()];
}

string genRandomName()
{
    random_device rd;
    vector<string> names;
    names.push_back("\\pm");
    names.push_back("\\cap");
    names.push_back("\\cup");
    names.push_back("\\diamond");
    names.push_back("\\oplus");
    names.push_back("\\mp");
    names.push_back("\\ominus");
    names.push_back("\\times");
    names.push_back("\\uplus");
    names.push_back("\\otimes");
    names.push_back("\\sqcup");
    names.push_back("\\sqcap");
    names.push_back("\\sqcup");
    names.push_back("\\amalg");
    names.push_back("\\star");
    names.push_back("\\vdash");
    names.push_back("\\Vdash");
    names.push_back("\\vDash");
    names.push_back("\\VDash");
    names.push_back("\\looparrowleft");
    names.push_back("\\looparrowright");
    names.push_back("\\forall");
    names.push_back("\\exists");
    names.push_back("\\aleph");
    names.push_back("\\imath");
    names.push_back("\\infty");
    names.push_back("\\clubsuit");
    names.push_back("\\spadesuit");
    names.push_back("\\mho");
    names.push_back("bla");
    names.push_back("\\LaTeX");
    names.push_back("RainbowDash");
    names.push_back("Fluttershy");
    names.push_back("Spike");
    names.push_back("Rarity");
    names.push_back("TwilightSparkle");
    names.push_back("AppleJack");
    names.push_back("PinkiePie");
    names.push_back("Celestia");
    names.push_back("Angel");
    names.push_back("Tank");
    names.push_back("Opal");
    names.push_back("Winona");
    names.push_back("Boulder");
    names.push_back("Owlowiscious");
    names.push_back("BigMac");
    names.push_back("Eeyup!");
    names.push_back("Nnope!");
    names.push_back("Flim");
    names.push_back("Flam");
    names.push_back("Flim\\&Flam");
    names.push_back("GranySmith");
    names.push_back("MaudPie");
    names.push_back("AppleBloom");
    names.push_back("Scootaloo");
    names.push_back("SweetieBelle");
    names.push_back("Aristophane");

    return names[rd()%names.size()];
}

string genTitle()
{
    random_device rd;
    vector<string> titles;
    titles.push_back("Des détails !");
    titles.push_back("De la beauté des preuves mathématiques");
    titles.push_back("J'aime les pommes de terre");
    titles.push_back("Convex Algebra");
    titles.push_back("Par moi, vous pénétrez dans la cité des peines ; \\\\"
            "par moi, vous pénétrez dans la douleur sans fin ;\\\\"
            "par moi, vous pénétrez parmi la gent perdue.\\\\"
            "La justice guidait la main de mon auteur ;\\\\"
            "le pouvoir souverain m'a fait venir au monde,\\\\"
            "la suprême sagesse et le premier amour.\\\\"
            "Nul autre objet créé n'existait avant moi,\\\\"
            "à part les éternels ; et je suis éternelle.\\\\"
            "Vous, qui devez entrer, abandonnez l'espoir. ");
    titles.push_back("La logique mène à tout, à condition d'en sortir");
    titles.push_back("Je suis une licorne !");
    titles.push_back("What does the fox say ?");
    titles.push_back("J'aime ce projet, man ! Et il est libre... man !\\\\ Et moi, ce qui est libre, j'en veux... Man !");
    titles.push_back("Vous permettez que je machouille mes lunettes ?");
    titles.push_back("Bonjour, en guise de cadeau de bienvenue, je vous offre une spécialité culinaire : une graine de sésame.");
    titles.push_back("Si tu tiens à tes rêves, personne ne peut t'empêcher de les vivre. Sauf si ces personnes ont des seringues de tranquillisant et des antipsychotiques");
    titles.push_back("T'as pas peur, toi !");
    titles.push_back("Aussi loin que je me souvienne, j'ai toujours été amnésique.");
    titles.push_back("Darwin serait fier de toi");
    titles.push_back("Ce projet est riant. Mercure vous proteste d’en amuser la cour céleste ; j’en attends un succès heureux.");
    titles.push_back("Viens chez le psy, c’est ton ami. Vas-y, viens me voir. Raconte-moi tes conneries.");
    titles.push_back("Où en est le cocotier ?");
    titles.push_back("Je crois qu’on peut vous applaudir, ces crimes sont exemplaires");
    titles.push_back("Généralement quand un suicidaire menace de sauter, moi je lui dit de laisser tomber.");
    titles.push_back("Les filaments Duchmont sont les meilleurs du monde. Les filaments Duchmont toutes les ampoules en demandent");
    titles.push_back("Nous avons retrouvé cette tablette faisant référence à un astre brillant apparu dans le ciel le temps d’une nuit d’été australe. Les indigènes l’appelèrent Xantahouhastikatiimaritatouhoupetchiikahmaal. Ce qui signifie \"astre brillant apparu dans le ciel le temps d’une nuit d’été australe\"");
    titles.push_back("Bla bli bla");
    titles.push_back("Essaie de prononcer <<~Dijkstra~>>");
    titles.push_back("Plus il y a de variables, plus on rit");
    titles.push_back("La déduction, c'est vraiment pas naturel");
    titles.push_back("$++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.$");
    titles.push_back("\\[\\int_{C^+}{P\\,\\mathrm{d}x + Q\\,\\mathrm{d}y} = \\iint_D\\left( \\frac{\\partial Q}{\\partial x} - \\frac{\\partial P}{\\partial y}\\right)  \\mathrm dx\\mathrm dy\\]");
    titles.push_back(" Génial !");
    titles.push_back("100\\% pur jus !");
    titles.push_back("Peut contenir des traces d'arachides !");
    titles.push_back("Sexy !");
    titles.push_back("Fabriqué par un Orang-Outan et Scrat.");
    titles.push_back("Fabriqué par Spike et Rainbow Dash.");
    titles.push_back("Le meilleur de la classe !");
    titles.push_back("Excitant !");
    titles.push_back("Regarde ça !");
    titles.push_back("La vache, mec !");
    titles.push_back("J'ai perdu...");
    titles.push_back("Compatible clavier !");
    titles.push_back("Non documenté !");
    titles.push_back("À 90\\% sans bugs !");
    titles.push_back("Ceci n'est pas un titre !");
    titles.push_back("Logique !");
    titles.push_back("{\\color{red}C}{\\color{yellow}o}{\\color{green}u}{\\color{cyan}l}{\\color{blue}e}{\\color{magenta}u}{\\color{red}r}{\\color{yellow}s}");
    titles.push_back("Essaye-le !");
    titles.push_back("Essaye le ragoût de champignons !");
    titles.push_back("Sensationnel !");
    titles.push_back("NP n'est pas P !");
    titles.push_back("Polynomial !");
    titles.push_back("Scientifique !");
    titles.push_back("Plus cool que Spock !");
    titles.push_back("$\\sqrt{-1}$ love you !");
    titles.push_back("20 GOTO 10 ");
    titles.push_back("Cogito ergo sum !");
    titles.push_back("4815162342 lignes de code !");
    titles.push_back("Supercalifragilisticexpialidocious !");
    titles.push_back("Turing-complet !");
    titles.push_back("Ne pas regarder directement les bugs !");
    titles.push_back("Enfin terminé !");
    titles.push_back("Riche en fonctionnalités !");
    titles.push_back("Ne pas éviter les doubles négations !");
    titles.push_back("Technologique !");
    titles.push_back("Quoi de neuf, docteur ?");
    titles.push_back("Contient maintenant, 32 chats journaliers aléatoires !");
    titles.push_back("Coucou, tu veux voir mon simplexe ?");
    titles.push_back("Coucou, tu veux voir mon gros dictionnaire ?");
    titles.push_back("Heisenbugs inside!");
    titles.push_back("Powered by quantum physics!");
    titles.push_back("Curry-Howard safe!");
    titles.push_back("Votre sécurité est ma seule priorité");
    //return titles[titles.size()-1];
    return titles[rd()%titles.size()];
}















//Bravo, vous avez trouvé la fonctionnalite cachee ! Demandez votre oeuf de Paques au prochain cours.



/**
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::##############                              :::::::::::::::::::
############################  ##############################  :::::::::::::::::
#########################  ######???????????????????????######  :::::::::::::::
=========================  ####??????????()????()?????????####  :::::::::::::::
=========================  ##????()??????????????    ()?????##  ::::    :::::::
------------=============  ##??????????????????  ;;;;  ?????##  ::  ;;;;  :::::
-------------------------  ##??????????()??????  ;;;;;;?????##    ;;;;;;  :::::
-------------------------  ##??????????????????  ;;;;;;         ;;;;;;;;  :::::
++++++++++++-------------  ##??????????????????  ;;;;;;;;;;;;;;;;;;;;;;;  :::::
+++++++++++++++++++++++++  ##????????????()??  ;;;;;;;;;;;;;;;;;;;;;;;;;;;  :::
+++++++++++++++++    ;;;;  ##??()????????????  ;;;;;;@@  ;;;;;;;;@@  ;;;;;  :::
~~~~~~~~~~~~~++++;;;;;;;;  ##????????????????  ;;;;;;    ;;;  ;;;    ;;;;;  :::
~~~~~~~~~~~~~~~  ;;  ~~~~  ####??????()??????  ;;[];;;;;;;;;;;;;;;;;;;;;[]  :::
$$$$$$$$$$$$$~~~~  ~~~~~~  ######?????????????  ;;;;;;              ;;;;  :::::
$$$$$$$$$$$$$$$$$$$$$$$$$    ###################  ;;;;;;;;;;;;;;;;;;;;  :::::::
$$$$$$$$$$$$$$$$$$$$$$$  ;;;;                                       :::::::::::
:::::::::::::$$$$$$$$$$  ;;;;  ::  ;;  ::::::::::::  ;;  ::  ;;;;  ::::::::::::
:::::::::::::::::::::::      ::::::    :::::::::::::     ::::      ::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::NN::::NN::YY::::YY:::AAAAAA:::NN::::NN:::!!::::::::::::::::::::
::::::::::::::::NNNN::NN::YY::::YY::AA::::AA::NNNN::NN:::!!::::::::::::::::::::
::::::::::::::::NNNN::NN::YY::::YY::AA::::AA::NNNN::NN:::!!::::::::::::::::::::
::::::::::::::::NN::NNNN::::YYYY::::AAAAAAAA::NN::NNNN:::!!::::::::::::::::::::
::::::::::::::::NN::NNNN:::::YY:::::AA::::AA::NN::NNNN:::::::::::::::::::::::::
::::::::::::::::NN::::NN:::::YY:::::AA::::AA::NN::::NN:::!!::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::YOU:HAVE:DONE:SIMPLEX:PROJECT:FOR:10368042:SECONDS!::::::::::::::::
**/
