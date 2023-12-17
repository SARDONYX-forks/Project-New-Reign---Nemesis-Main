#include "hkx/HkxBehaviorFile.h"

#include "generate/animation/singletemplate.h"

#include "template/processparser.h"

#include "utilities/animquery.h"
#include "utilities/lexersearch.h"
#include "utilities/lineprocess.h"
#include "utilities/process.h"
#include "utilities/template.h"
#include "utilities/templatecategory.h"

using namespace std;
namespace sf = std::filesystem;

const nemesis::Template& nemesis::Template::Exporter::GetSelf()
{
    return static_cast<const nemesis::Template&>(file);
}

nemesis::Template::Exporter::Exporter(const nemesis::Template& templateref, VecNstr& storeline)
    : storeline(storeline)
    , nemesis::Exporter(templateref)
{
}

void nemesis::Template::Exporter::ExportCurrentQuery(const nemesis::AnimQuery& query)
{
    scopeinfo->SetCurrentQuery(query);
    auto& self = GetSelf();

    if (!self.IsGroup())
    {
        scopeinfo->InsertAnim(query, &self.GetTemplateCategory());
    }

    if (!self.IsMaster())
    {
        scopeinfo->InsertQuery(query, &self.GetTemplateCategory());
    }

    Export();
}

const nemesis::AnimTemplate* nemesis::Template::Exporter::GetAnimTemplate()
{
    return &GetSelf().GetTemplateCategory().GetAnimTemplate();
}

const nemesis::TemplateCategory* nemesis::Template::Exporter::GetTemplateCategory(const std::string& name)
{
    auto& self = GetSelf();

    if (nemesis::iequals(self.GetTemplateCategory().GetName(), name)) return &self.GetTemplateCategory();

    return GetAnimTemplate()->GetClass(name);
}

void nemesis::Template::Exporter::Export()
{
    auto& self = GetSelf();

    for (auto& line : self.contents)
    {
        VecNstr compiledlines = line->GetCompiledData<nemesis::Line>(*scopeinfo);
        storeline.insert(storeline.end(), compiledlines.begin(), compiledlines.end());
    }

    for (auto& line : storeline)
    {
        nemesis::Exporter::CheckNumElement(line);
    }

    storeline.emplace_back(std::string());
}

bool nemesis::Template::Exporter::IsTemplateActive(const std::string& name)
{
    throw std::runtime_error("Template does not support checking the active status of any other template");
}

const Vec<const nemesis::AnimQuery*>*
nemesis::Template::Exporter::GetQueriesByTemplate(const std::string& name)
{
    auto& self = GetSelf();
    ErrorMessage(1227, self.GetTemplateCategory().GetName(), self.GetFilePath());
    return nullptr;
}

sf::path nemesis::Template::Parser::GetFilePath() const noexcept
{
    return rTemplate.GetFilePath();
}

void nemesis::Template::Parser::GetFileLines()
{
    filelines = file.GetLines();
}

void nemesis::Template::Parser::PrepareAllRegexes()
{
    PrepareStateIdRegex();
    PrepareNodeIdRegex();
    PrepareAnimObjRegex();
}

void nemesis::Template::Parser::PrepareStateIdRegex()
{
    auto classname = rTemplate.GetFileClassName();

    std::stringstream ss;
    ss << "\\$.*?((?:(";
    ss << classname;
    ss << ")(?:_group\\[([0-9]*)\\]|)\\[(F|N|L|B|[0-9]*)\\]\\[S@(?=[0-9]+\\]\\$)|S@(?=[0-9]+\\$))([0-9]+)\\]?"
          ").*?\\$";
    stateid_rgx = std::make_unique<nemesis::regex>(ss.str());
}

void nemesis::Template::Parser::PrepareNodeIdRegex()
{
    auto classname = std::string(rTemplate.GetFileClassName());
    AddMasterNodeIdRegex(classname);
    AddGroupNodeIdRegex(classname);
    AddAnimNodeIdRegex(classname);
}

void nemesis::Template::Parser::PrepareAnimObjRegex()
{
    std::stringstream ss;
    ss << "\\$.*?((";
    ss << rTemplate.GetFileClassName();
    ss << ")(?:_group\\[([0-9]*)\\]|)\\[(F|N|L|B|[0-9]*)\\]\\[AnimObject/[0-9]+\\]|AnimObject/"
          "[0-9]+).*?\\$";
    animobj_rgx = std::make_unique<nemesis::regex>(ss.str());
}

void nemesis::Template::Parser::PrepareAllLexers()
{
    if (rTemplate.IsImport()) return;

    PrepareVariableLexer();

    auto templtclass = &rTemplate.GetTemplateCategory();
    PrepareLexer("main_anim_event",
                 [templtclass](nemesis::ScopeInfo& scopeinfo)
                 { return std::string(scopeinfo.GetAnim(templtclass)->GetAnimationName()); });
    PrepareLexer("FilePath",
                 [templtclass](nemesis::ScopeInfo& scopeinfo)
                 { return scopeinfo.GetAnim(templtclass)->GetAnimPath().string(); });
    PrepareLexer("Index",
                 [templtclass](nemesis::ScopeInfo& scopeinfo)
                 { return std::to_string(scopeinfo.GetAnim(templtclass)->GetBehaviorIndex()); });
    PrepareLexer("GroupIndex",
                 [templtclass](nemesis::ScopeInfo& scopeinfo)
                 { return std::to_string(scopeinfo.GetAnim(templtclass)->GetIndex()); });
    PrepareLexer("ArrayIndex",
                 [templtclass](nemesis::ScopeInfo& scopeinfo)
                 { return std::to_string(scopeinfo.GetAnim(templtclass)->GetArrayIndex()); });
}

void nemesis::Template::Parser::PrepareVariableLexer()
{
    auto classname = rTemplate.GetFileClassName();

    for (auto& option : rTemplate.GetTemplateCategory().GetOptionModels().GetOptionList())
    {
        auto optname = option->GetName();

        for (auto& varptr : option->GetVariablesList())
        {
            auto varname = varptr->GetName();
            varlexer_list.emplace_back(std::make_pair(varname,
                                                      nemesis::AnimVarPtr::Lexer(std::string(classname),
                                                                                 std::string(optname),
                                                                                 std::string(varname))));
        }
    }
}

void nemesis::Template::Parser::PrepareLexer(const std::string& keyword, RtnFunc callback)
{
    auto uptr = std::make_unique<LexerSearch>(keyword);
    uptr->AddLexer(std::string(rTemplate.GetFileClassName()), callback);
    lexersearch_list.emplace_back(std::move(uptr));
}

void nemesis::Template::Parser::ParseLines(const VecNstr& lineList)
{
    size_t i = 0;
    stream.clear();
    stream.emplace_back(&rTemplate.contents);

    for (auto& line : lineList)
    {
        if (error) throw nemesis::exception();

        ParseLine(line);
    }
}

void nemesis::Template::Parser::TryCacheData(const nemesis::Line& line,
                                             const nemesis::ConditionInfo* conditioninfo)
{
    processparser = std::make_unique<nemesis::ProcessParser>(*this);
    processparser->SetLine(line);
    processparser->Parse();
}

void nemesis::Template::Parser::AddConditionScope(const nemesis::ConditionInfo* conditioninfo)
{
    size_t linenum    = conditioninfo->GetLineNumber();
    auto filepath_ptr = std::make_shared<nemesis::SharableWrapper<std::filesystem::path>>(file.GetFilePath());
    nemesis::Line conditionstr(conditioninfo->GetCondition(), linenum, filepath_ptr);
    auto condptr = std::make_shared<nemesis::Condition>(
        conditionstr, conditionstr, rTemplate, conditioninfo->GetType());
    auto condvar    = nemesis::CondVar<nemesis::PreprocessLine>(conditioninfo->GetType(), condptr);
    condvar.linenum = linenum;
    SPtr<nemesis::LinkedPreprocessLine> linkedline;
    linkedline->AddCondition(*conditioninfo, file);
    AddLinkedLine(linkedline);
    LevelUpScope();
}

void nemesis::Template::Parser::PostAddLineProcess(nemesis::LinkedPreprocessLine& linkedline)
{
    if (linkedline.HasCondition() || linkedline.GetRawPtr()->empty()) return;

    linkedline.GetRawPtr()->AddTemplate(rTemplate);

    if (!processparser || processparser->IsEmpty()) return;

    processparser->ToLine(*linkedline.GetRawPtr());
}

nemesis::Template::Parser::Parser(Template& host)
    : rTemplate(host)
    , nemesis::HkxParserBase(host)
{
    PrepareAllRegexes();
    PrepareAllLexers();
}

void nemesis::Template::Parser::ReadFile(const sf::path& filepath)
{
    DebugLogging(L"Parsing template: " + filepath.wstring());
    auto code    = rTemplate.GetFileClassName();
    this->cscope = std::make_unique<nemesis::ConditionScope>(std::string(code), filepath);
    GetFileLines();
}

void nemesis::Template::SetFilePath(const sf::path& filepath)
{
    this->filepath = filepath;
    SetBehaviorFile();
    SetTemplateType();
}

void nemesis::Template::SetBehaviorFile()
{
    // import file does not have behavior file
    if (IsImport()) return;

    std::wstring pathstr = filepath.wstring();
    nemesis::to_lower(pathstr);
    std::wstring name = nemesis::transform_to(pTemplateCategory->GetName().ToString()) + L"\\";
    behaviorfile      = sf::path(pathstr.substr(pathstr.rfind(name) + name.length()));
    behaviorfile      = behaviorfile.substr(0, behaviorfile.find(L"\\"));
    nemesis::to_lower(behaviorfile);
}

void nemesis::Template::SetTemplateType()
{
    std::wstring lowername    = nemesis::to_lower_copy(filepath.stem().wstring());
    const std::wstring master = L"_master";
    const std::wstring group  = L"_group";

    if (lowername.rfind(master) == lowername.length() - master.length())
    {
        type = FileType::MASTER;
    }
    else if (lowername.rfind(group) == lowername.length() - group.length())
    {
        type = FileType::GROUP;
    }
    else
    {
        type = FileType::SINGLE;
    }
}

nemesis::Template::Template(const nemesis::TemplateCategory& templtclass) noexcept
{
    pTemplateCategory = &templtclass;
    classname         = templtclass.GetName();
}

bool nemesis::Template::IsGroup() const noexcept
{
    return type != FileType::SINGLE;
}

bool nemesis::Template::IsMaster() const noexcept
{
    return type == FileType::MASTER;
}

bool nemesis::Template::IsImport() const noexcept
{
    return classname == GetImporterStr();
}

nemesis::File::FileType nemesis::Template::GetType() const noexcept
{
    return type;
}

const Vec<SPtr<nemesis::LinkedPreprocessLine>> nemesis::Template::GetContents() const noexcept
{
    return contents;
}

std::wstring nemesis::Template::GetBehaviorFileW() const noexcept
{
    return behaviorfile;
}

const Vec<int>& nemesis::Template::GetStateMultiplier() const noexcept
{
    return statemultiplier;
}

const nemesis::HkxBehaviorFile& nemesis::Template::GetBehavior() const noexcept
{
    return *hkxbehavior;
}

const nemesis::TemplateCategory& nemesis::Template::GetTemplateCategory() const noexcept
{
    return *pTemplateCategory;
}

void nemesis::Template::GetQueryResult(const nemesis::AnimQuery& query,
                                       VecNstr& storeline,
                                       nemesis::Exporter& exptr) const
{
    if (!exporter)
    {
        exporter = std::make_unique<nemesis::Template::Exporter>(*this, storeline);
        exporter->SetInnerExporter(exptr);
    }

    exporter->ExportCurrentQuery(query);
}

void nemesis::Template::GetImportResult(VecNstr& storeline, nemesis::Exporter& exptr) const
{
    exporter = std::make_unique<nemesis::Template::Exporter>(*this, storeline);
    exporter->SetInnerExporter(exptr);
    exporter->Export();
}

void nemesis::Template::ReadFile(const sf::path& filepath)
{
    SetFilePath(filepath);
    Parser parser(*this);
    parser.ReadFile(filepath);
    parser.ParseFile();
}

void nemesis::Template::AddBehavior(nemesis::HkxBehaviorFile& behavior) const noexcept
{
    hkxbehavior = &behavior;
}

bool nemesis::Template::TryAddBehavior(nemesis::HkxBehaviorFile& behavior) const
{
    if (!nemesis::iequals(behavior.GetBehaviorName(), behaviorfile)) return false;

    behavior.AddTemplate(*this);
    AddBehavior(behavior);
    return true;
}

bool nemesis::Template::TryAddBehaviorList(const VecSPtr<nemesis::HkxBehaviorFile>& behaviorlist) const
{
    for (auto& each : behaviorlist)
    {
        if (!nemesis::iequals(each->GetBehaviorName(), behaviorfile)) continue;

        each->AddTemplate(*this);
        AddBehavior(*each);
        return true;
    }

    return false;
}

SPtr<nemesis::Template> nemesis::Template::CreateImport(const std::filesystem::path& filepath)
{
    SPtr<Template> tempptr(new Template());
    tempptr->classname = GetImporterStr();
    tempptr->type      = FileType::IMPORT;
    tempptr->ReadFile(filepath);
    return tempptr;
}
