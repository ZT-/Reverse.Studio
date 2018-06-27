using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Windows;
using Caliburn.Micro;
using Gemini.Framework;
using Gemini.Framework.Menus;
using Gemini.Modules.Inspector;
using Gemini.Modules.Output;
using Virtion.Knife.Modules.PeEditor.Commands;
using Virtion.Knife.Modules.PeEditor.ViewModels;

namespace Virtion.Knife.Modules.PeEditor
{
    [Export(typeof(IModule))]
    public class Module : ModuleBase
    {
        [Export]
        public static MenuItemGroupDefinition ViewDemoMenuGroup = new MenuItemGroupDefinition(
            Gemini.Modules.MainMenu.MenuDefinitions.ToolsMenu, 10);

        [Export]
        public static MenuItemDefinition ViewHomeMenuItem = new CommandMenuItemDefinition<ViewHomeCommandDefinition>(
            ViewDemoMenuGroup, 0);

        [Export]
        public static MenuItemDefinition ViewAdbMenuItem = new CommandMenuItemDefinition<ViewAdbCommandDefinition>(
            ViewDemoMenuGroup, 1);


        public static IOutput Output;
        private readonly IInspectorTool _inspectorTool;

        [ImportingConstructor]
        public Module(IOutput output, IInspectorTool inspectorTool)
        {
            Output = output;
            _inspectorTool = inspectorTool;
        }

        public override IEnumerable<IDocument> DefaultDocuments
        {
            get
            {
                yield return IoC.Get<HomeViewModel>();
            }
        }

        public override void PostInitialize()
        {
            //IoC.Get<IPropertyGrid>().SelectedObject = IoC.Get<HomeViewModel>();
            //Shell.OpenDocument(IoC.Get<HomeViewModel>());
        }
    }
}
