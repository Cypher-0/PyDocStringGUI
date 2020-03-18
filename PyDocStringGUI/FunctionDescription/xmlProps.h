#ifndef XMLPROPS_H
#define XMLPROPS_H

#include <QString>

namespace XML
{

namespace XMLProps
{
    namespace Func
    {
        static const inline QString node_funcDescRoot{"FunctionDescription"};
                static const inline QString attrs_name{"name"};
            static const inline QString node_desc{"description"};
            static const inline QString node_list_args{"list_args"};
            static const inline QString node_list_returnArgs{"list_returnArgs"};
    }

    namespace Arg
    {
        static const inline QString node_argRoot{"Argument"};
                static inline QString attrs_name{"argName"};
                static inline QString attrs_type{"argType"};
            static inline QString node_desc{"argDesc"};
    }
};

};

#endif // XMLPROPS_H
