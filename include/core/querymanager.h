#pragma once

#include "utilities/animquery.h"
#include "utilities/animqueryfile.h"
#include "utilities/templatecategory.h"
#include "utilities/types.h"

namespace nemesis
{
    struct AnimQuery;
    struct AnimQueryFile;
    struct TemplateCategory;

    struct QueryManager
    {
    private:
        Vec<SPtr<const AnimQueryFile>> filelist;
        Vec<const AnimQueryFile*> fileptr_list;
        Map<const TemplateCategory*, Vec<const AnimQuery*>> template_querylist;

    public:
        void AddFile(SPtr<const AnimQueryFile> fileptr);

        const Vec<const AnimQuery*>* GetQueryList(const TemplateCategory* templtclass) const;
        const Vec<const AnimQueryFile*>& GetFileList() const noexcept;
    };
} // namespace nemesis
