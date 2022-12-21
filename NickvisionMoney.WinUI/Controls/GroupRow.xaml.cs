using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using NickvisionMoney.Shared.Helpers;
using NickvisionMoney.Shared.Models;
using System;
using Windows.UI;

namespace NickvisionMoney.WinUI.Controls;

/// <summary>
/// A row to display a Group model
/// </summary>
public sealed partial class GroupRow : UserControl
{
    private readonly Group _group;

    /// <summary>
    /// The Id of the Group the row represents
    /// </summary>
    public uint Id => _group.Id;

    /// <summary>
    /// Occurs when the filter checkbox is changed on the row
    /// </summary>
    public event EventHandler<(int Id, bool Filter)>? FilterChanged;
    /// <summary>
    /// Occurs when the edit button on the row is clicked 
    /// </summary>
    public event EventHandler<uint>? EditTriggered;
    /// <summary>
    /// Occurs when the delete button on the row is clicked 
    /// </summary>
    public event EventHandler<uint>? DeleteTriggered;

    /// <summary>
    /// Constructs a GroupRow
    /// </summary>
    /// <param name="group">The Group model to represent</param>
    /// <param name="localizer">The Localizer for the app</param>
    public GroupRow(Group group, Localizer localizer)
    {
        InitializeComponent();
        _group = group;
        if(_group.Id == 0)
        {
            BtnEdit.Visibility = Visibility.Collapsed;
            BtnDelete.Visibility = Visibility.Collapsed;
        }
        else
        {
            //Localize Strings
            ToolTipService.SetToolTip(BtnEdit, localizer["Edit", "GroupRow"]);
            ToolTipService.SetToolTip(BtnDelete, localizer["Delete", "GroupRow"]);
        }
        //Load Group
        ChkFilter.IsChecked = true;
        LblName.Text = _group.Name;
        LblDescription.Text = _group.Description;
        LblAmount.Text = _group.Balance.ToString("C");
        LblAmount.Foreground = _group.Balance >= 0 ? new SolidColorBrush(ActualTheme == ElementTheme.Light ? Color.FromArgb(255, 38, 162, 105) : Color.FromArgb(255, 143, 240, 164)) : new SolidColorBrush(ActualTheme == ElementTheme.Light ? Color.FromArgb(255, 192, 28, 40) : Color.FromArgb(255, 255, 123, 99));
    }

    /// <summary>
    /// Whether or not the filter checkbox is active
    /// </summary>
    public bool FilterActive
    {
        get => ChkFilter.IsChecked ?? false;

        set => ChkFilter.IsChecked = value;
    }

    /// <summary>
    /// Occurs when the filter checkbox is changed on the row
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ChkFilterChanged(object sender, RoutedEventArgs e) => FilterChanged?.Invoke(this, ((int)Id, ChkFilter.IsChecked ?? false));

    /// <summary>
    /// Occurs when the edit button on the row is clicked 
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Edit(object sender, RoutedEventArgs e) => EditTriggered?.Invoke(this, Id);

    /// <summary>
    /// Occurs when the delete button on the row is clicked 
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Delete(object sender, RoutedEventArgs e) => DeleteTriggered?.Invoke(this, Id);
}
