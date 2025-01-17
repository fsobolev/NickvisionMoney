﻿using NickvisionMoney.Shared.Helpers;
using NickvisionMoney.Shared.Models;
using System;

namespace NickvisionMoney.Shared.Controllers;

/// <summary>
/// A controller for a PreferencesView
/// </summary>
public class PreferencesViewController
{
    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;

    /// <summary>
    /// Occurs when the recent accounts list is changed
    /// </summary>
    private event EventHandler? RecentAccountsChanged;

    /// <summary>
    /// Creates a PreferencesViewController
    /// </summary>
    /// <param name="recentAccountsChanged">The recent accounts changed event</param>
    /// <param name="localizer">The Localizer of the app</param>
    internal PreferencesViewController(EventHandler? recentAccountsChanged, Localizer localizer)
    {
        Localizer = localizer;
        RecentAccountsChanged = recentAccountsChanged;
    }

    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme
    {
        get => Configuration.Current.Theme;

        set => Configuration.Current.Theme = value;
    }

    /// <summary>
    /// The default color of a transaction
    /// </summary>
    public string TransactionDefaultColor
    {
        get => Configuration.Current.TransactionDefaultColor;

        set => Configuration.Current.TransactionDefaultColor = value;
    }

    /// <summary>
    /// The default color of a transfer
    /// </summary>
    public string TransferDefaultColor
    {
        get => Configuration.Current.TransferDefaultColor;

        set => Configuration.Current.TransferDefaultColor = value;
    }

    /// <summary>
    /// The color of accounts with Checking type
    /// </summary>
    public string AccountCheckingColor
    {
        get => Configuration.Current.AccountCheckingColor;

        set => Configuration.Current.AccountCheckingColor = value;
    }

    /// <summary>
    /// The color of accounts with Savings type
    /// </summary>
    public string AccountSavingsColor
    {
        get => Configuration.Current.AccountSavingsColor;

        set => Configuration.Current.AccountSavingsColor = value;
    }

    /// <summary>
    /// The color of accounts with Business type
    /// </summary>
    public string AccountBusinessColor
    {
        get => Configuration.Current.AccountBusinessColor;

        set => Configuration.Current.AccountBusinessColor = value;
    }

    /// <summary>
    /// Saves the configuration to disk
    /// </summary>
    public void SaveConfiguration()
    {
        Configuration.Current.Save();
        RecentAccountsChanged?.Invoke(this, EventArgs.Empty);
    }
}