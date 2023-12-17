#pragma once

#include <deque>

#include "utilities/conditiondetails.h"
#include "utilities/conditionscope.h"
#include "utilities/line.h"

#include "update/animsetdata/animsetcrc32pack.h"
#include "update/animsetdata/animsetpack.h"

struct AnimSetData
{
    using DataStr = std::string;
    using ModCode = std::string;

    struct WeapType
    {
        nemesis::LinkedVar<std::string> name;
        nemesis::LinkedVar<std::string> from;
        nemesis::LinkedVar<std::string> to;

        void Getlines(VecStr& storeline);

        bool AddName(const nemesis::Line& _name);
        bool AddFrom(const nemesis::Line& _from);
        bool AddTo(const nemesis::Line& _to);
    };

    nemesis::LinkedVar<std::string> name;
    Vec<nemesis::LinkedVar<DataStr>> equiplist;
    Vec<nemesis::LinkedVar<WeapType>> typelist;
    Vec<nemesis::LinkedVar<AnimSetPack>> animlist;
    Vec<nemesis::LinkedVar<AnimSetCRC32Pack>> crc32list;

    AnimSetData(std::string _name, size_t num);

    void ImportData(std::filesystem::path path, size_t& num, const VecNstr& storeline);

    void ImportData(std::filesystem::path path, size_t& num, const VecNstr& storeline, const ModCode& format);

    void ImportDataTemplate(std::filesystem::path path,
                            size_t& num,
                            const VecNstr& storeline,
                            const ModCode& format);

    void Getlines(VecStr& storeline) const;
    void Getname(VecStr& storeline);
    std::string Getfirstname();
    std::string Getfirstname(nemesis::LinkedVar<std::string> _name);

private:
    SPtr<DataStr>& AddEquip(const DataStr& equip, size_t num);
    SPtr<DataStr>& AddEquip(const DataStr& equip, size_t num, const nemesis::CondDetails& condition);
    SPtr<DataStr>& AddEquip(const DataStr& equip, size_t num, const nemesis::ConditionInfo& conditioninfo);

    SPtr<WeapType>& AddType(const WeapType& type, size_t num);
    SPtr<WeapType>& AddType(const WeapType& type, size_t num, const nemesis::CondDetails& condition);
    SPtr<WeapType>& AddType(const WeapType& type, size_t num, const nemesis::ConditionInfo& conditioninfo);

    SPtr<AnimSetPack>& AddAnimPack(const AnimSetPack& animpack, size_t num);
    SPtr<AnimSetPack>&
    AddAnimPack(const AnimSetPack& animpack, size_t num, const nemesis::CondDetails& condition);
    SPtr<AnimSetPack>&
    AddAnimPack(const AnimSetPack& animpack, size_t num, const nemesis::ConditionInfo& conditioninfo);

    SPtr<AnimSetCRC32Pack>& AddCrc32Pack(const AnimSetCRC32Pack& crc32pack, size_t num);
    SPtr<AnimSetCRC32Pack>&
    AddCrc32Pack(const AnimSetCRC32Pack& crc32pack, size_t num, const nemesis::CondDetails& condition);
    SPtr<AnimSetCRC32Pack>&
    AddCrc32Pack(const AnimSetCRC32Pack& crc32pack, size_t num, const nemesis::ConditionInfo& conditioninfo);

    template <typename BaseData>
    static SPtr<BaseData>&
    AddData(Vec<nemesis::LinkedVar<BaseData>>& tarGet, const BaseData& type, size_t num);

    template <typename BaseData>
    static SPtr<BaseData>& AddData(Vec<nemesis::LinkedVar<BaseData>>& tarGet,
                                   const BaseData& data,
                                   size_t num,
                                   const nemesis::CondDetails& condition);

    template <typename BaseData>
    static SPtr<BaseData>& AddData(Vec<nemesis::LinkedVar<BaseData>>& tarGet,
                                   const BaseData& data,
                                   size_t num,
                                   const nemesis::ConditionInfo& conditioninfo);

    template <typename BaseData>
    static SPtr<BaseData>& AddData(Vec<nemesis::LinkedVar<BaseData>>& tarGet,
                                   const Vec<nemesis::LinkedVar<BaseData>>& datalist,
                                   size_t num,
                                   const nemesis::ConditionInfo& conditioninfo);

    void ExportEquip(VecStr& storeline) const;
    void ExportType(VecStr& storeline) const;
    void ExportAnimPack(VecStr& storeline) const;
    void ExportCrc32Pack(VecStr& storeline) const;

    class Parser
    {
        struct EquipConditionScope : public nemesis::ConditionScope
        {
            EquipConditionScope(const std::string& _modcode, const std::filesystem::path& _path)
                : nemesis::ConditionScope(_modcode, _path)
            {
            }
        };

        mutable size_t num;
        mutable Vec<nemesis::CondDetails> condtype;

        short type;
        size_t index;
        size_t iindex;
        size_t size;
        Vec<size_t> points;

        std::string format;
        std::filesystem::path path;
        nemesis::CondCheckFunc condfunc;

        AnimSetData& host;
        const VecNstr& storeline;
        SPtr<nemesis::ConditionInfo> tobedeleted;

        Vec<Vec<nemesis::LinkedVar<DataStr>>*> equipstream;

    public:
        Parser(AnimSetData& base, const VecNstr& _storeline);

        void SetFormat(const std::string& _format) noexcept;
        void SetPath(const std::filesystem::path& _path) noexcept;
        void SetStartIndex(const size_t& startIndex) noexcept;
        void SetCondCheckFunc(const nemesis::CondCheckFunc& _condfunc) noexcept;

        void AddEquipList();
        void AddTypeList();
        void AddAttackSetDataList();
        void AddCrc32PackList();

        void ImportEquip();
        void ImportType();
        void ImportAttackSetData();
        void ImportCrc32Pack();

        size_t GetNum() const noexcept;

    private:
        size_t GetEndAttackSetData();

        bool ImportNewWeapType(Deq<nemesis::Line>* edits);
        bool ImportNewAnimPack(Deq<nemesis::Line>* edits, Vec<size_t>& npoints);
        bool ImportNewCRC32Pack(Deq<nemesis::Line>* edits);

        void PointingEquip() noexcept;
        bool PointingType(const nemesis::Line& line, size_t i);
        void PointingAttackSetData(const nemesis::Line& line);
        void PointingCrc32Pack() noexcept;

        void PointingOriEquip();
        void PointingOriType();
        void PointingOriAttackSetData(const nemesis::Line& line);
        void PointingOriCrc32Pack();

        void CloseEquip();
        void CloseType(const nemesis::Line& line);
        void CloseAttackSetData(const nemesis::Line& line);
        void CloseCrc32Pack();

        Vec<size_t> GetAttackSetDataSections(size_t start, size_t end) const;
        Vec<size_t> GetAttackSetDataSections(size_t start, size_t end, const VecNstr& _storeline) const;
        Vec<size_t>
        GetAttackSetDataSections(size_t start, size_t end, const Deq<nemesis::Line>& _storeline) const;
    };
};

void getLinkedLines(const nemesis::LinkedVar<AnimSetData>& linkeddata, VecStr& storeline, VecStr& namelist);
