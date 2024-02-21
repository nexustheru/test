#include "monos.h"


Monoi::Monoi()
{
	mono_set_dirs("C:\\Program Files\\Mono\\lib", "C:\\Program Files\\Monoo\\etc");
    scripts = new std::vector<std::string>;
    dlls = new std::vector<std::string>;
    assemblys=new std::vector<MonoAssembly*>;
    images=new std::vector<MonoImage*>;
    classes=new std::vector<MonoClass*>;
    log = new Loggin();


}
void Monoi::Update()
{

}
void Monoi::Get_Scripts()
{
    
    std::string folderpath = "Scripts/";
    try
    {
        for (const auto& entry : fs::directory_iterator(folderpath))
        {
            if (fs::is_regular_file(entry))
            {
                if (entry.path().filename().extension().string() == ".cs")
                {
                    auto temp = folderpath + entry.path().filename().string();
                    scripts->push_back(temp);
                    std::cout << "adding " << temp << std::endl;
                }              
               
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;

    }
}
void Monoi::Get_Dlls()
{
 
    std::string folderpath = "Scripts/";

        for (const auto& entry : fs::directory_iterator(folderpath))
        {
            if (fs::is_regular_file(entry))
            {
               
                if (entry.path().filename().extension().string() == ".dll")
                {
                    auto temp = folderpath + entry.path().filename().string();
                    dlls->push_back(temp);
                    std::cout << "adding " << temp << std::endl;
                }
               
            }
        }
   
}

void Monoi::GetAssemblyClassList(MonoImage* image)
{
   
    const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

    int rows = mono_table_info_get_rows(table_info);

    /* For each row, get some of its values */
    for (int i = 0; i < rows; i++)
    {
        MonoClass* _class = nullptr;
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        _class = mono_class_from_name(image, name_space, name);
        classes->push_back(_class);
        if (strcmp(name, "<Module>") == 0) 
        {
            continue;
        }
    }

}
void Monoi::Init()
{

    domain = mono_jit_init("testapp");
    if (!domain)
    {
        std::cout << "mono_jit_init failed" << std::endl;
    }
    else
    {
        isRunning = true;
        std::cout << "starting to load modules up" << std::endl;
        sorting();

    }
  
}
bool Monoi::compile()
{
        for (int i = 0; i < scripts->size(); i++)
        {

            auto temp = scripts->at(i);
            std::string comexe = "mcs -target:library ";
            std::string command = comexe + temp;
           
            int compileResult = system(command.c_str());
            if (compileResult != 0)
            {
                std::cerr << "Compilation failed!" << std::endl;
                return false;
            }
            std::cout << "Compilation on " << temp << std::endl;
        }
    
   
 return true;
}
void Monoi::sorting()
{
 
    for (int i = 0; i < dlls->size(); i++)
    {
            auto temp = dlls->at(i);     
            assembly = mono_domain_assembly_open(domain, temp.c_str());
            if (!assembly)
            {
                std::cout << "mono_domain_assembly_open failed" << std::endl;
               
            }
            else
            {
                assemblys->push_back(assembly);
                std::cout << "assembly collected" << std::endl;
                image = mono_assembly_get_image(assembly);
                if (!image)
                {
                    std::cout << "mono_image failed" << std::endl;
                }
                else
                {
                    images->push_back(image);
                    std::cout << "image collected" << std::endl;
                    GetAssemblyClassList(image);
                    std::cout << "classes collected" << std::endl;
                }
               
            }
           
    }
      
    listfunc();
}
void Monoi::listfunc()
{
    for (int i = 0; i < classes->size(); i++)
    {
        auto cl = classes->at(i);
       
        std::cout << mono_class_get_name(cl) << std::endl;
        output_methods(cl);
        std::cout << "loading up plugin system mono" << std::endl;
      
        /*void* args[1];
        std::string pluginame = "Leplugin";
        args[0] = &pluginame;
        PluginsetPlugiName(args);*/
             
    }
}
void Monoi::createFunction(MonoImage* image, std::string classname,std::string classnameandMethod, void* args[])
{
    try
    {
    MonoClass* funcClass;
    funcClass = mono_class_from_name(image, "", classname.c_str());
    MonoObject* funcObject;
    funcObject = mono_object_new(domain, funcClass);
    mono_runtime_object_init(funcObject);
    MonoObject* result;
    MonoMethodDesc* FuncMethodDesc;
    char* FuncMethodDescStr = (char*)classnameandMethod.c_str();
    FuncMethodDesc = mono_method_desc_new(FuncMethodDescStr, NULL);
   
    //Search the method in the image
    MonoMethod* method;
    method = mono_method_desc_search_in_image(FuncMethodDesc, image);
    if (sizeof(args) > 0)
    {
        mono_runtime_invoke(method, funcObject, args, nullptr);
    }
    else
    {
      mono_runtime_invoke(method, funcObject, nullptr, nullptr);
    }
    }
    catch (std::exception ex)
    {
        log->logtofile(ex.what());
        std::cout << ex.what() << std::endl;
    }
   
    //Set the arguments for the method
    //void* args[1];
    //std::string barkTimes = "Leplugin";
    //args[0] = &barkTimes;

    //Run the method
  
   
}
void* Monoi::custom_malloc(size_t bytes)
{
    ++malloc_count;
    return malloc(bytes);
}
void  Monoi::output_methods(MonoClass* klass)
{
    MonoMethod* method;
    void* iter = NULL;

    while ((method = mono_class_get_methods(klass, &iter)))
    {
        UINT32 flags, iflags;
        flags = mono_method_get_flags(method, &iflags);
        printf("Method: %s, flags 0x%x, iflags 0x%x\n",
        mono_method_get_name(method), flags, iflags);
    }
}

void Monoi::testiter1(MonoClass* klass)
{
 void* iter = NULL;
    MonoClassField* field = mono_class_get_fields(klass, &iter);
    while (field != NULL)
    {
        std::string fieldName = mono_field_get_name(field);
        MonoCustomAttrInfo* info = mono_custom_attrs_from_field(klass, field);

        // If info is null, no attributes
        if (info == 0)
        {
            field = mono_class_get_fields(klass, &iter);
            continue;
        }
    }
}

Monoi::~Monoi()
{
    isRunning = false;
    mono_jit_cleanup(domain);
    std::cout << "exited mono" << std::endl;

}

//plugin method
void Monoi::PluginOnCreate(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:OnCreate()", args);
    
}
void Monoi::PluginOnUpdate(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:OnUpdate()", args);
  
}
void Monoi::PluginOnLoad(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:OnLoad()", args);
  
}
void Monoi::PluginOnExit(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:OnExit()", args);
    
}
void Monoi::PluginReturnName(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:ReturnName()", args);
}
void Monoi::PluginsetPlugiName(void* args[])
{
    createFunction(images->at(0), "Plugin", "Plugin:setPlugiName(string)" , args);
}
