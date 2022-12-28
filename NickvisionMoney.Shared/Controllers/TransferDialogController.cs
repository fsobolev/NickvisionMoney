﻿using NickvisionMoney.Shared.Helpers;
using NickvisionMoney.Shared.Models;
using System.Globalization;
using System.IO;

namespace NickvisionMoney.Shared.Controllers;


/// <summary>
/// Statuses for when a transfer is validated
/// </summary>
public enum TransferCheckStatus
{
    Valid = 0,
    InvalidDestPath,
    InvalidAmount
}

/// <summary>
/// A controller for a TransferDialog
/// </summary>
public class TransferDialogController
{
    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }
    /// <summary>
    /// The transfer represented by the controller
    /// </summary>
    public Transfer Transfer { get; init; }
    /// <summary>
    /// Whether or not the dialog was accepted (response)
    /// </summary>
    public bool Accepted { get; set; }

    /// <summary>
    /// Constructs a TransferDialogController
    /// </summary>
    /// <param name="transfer">The Transfer model</param>
    /// <param name="localizer">The Localizer for the app</param>
    public TransferDialogController(Transfer transfer, Localizer localizer)
    {
        Localizer = localizer;
        Transfer = transfer;
        Accepted = false;
    }

    /// <summary>
    /// Updates the Transfer object
    /// </summary>
    /// <param name="destPath">The new path of the destination account</param>
    /// <param name="amountString">The new amount string</param>
    /// <returns>TransferCheckStatus</returns>
    public TransferCheckStatus UpdateTransfer(string destPath, string amountString)
    {
        var amount = 0m;
        if(string.IsNullOrEmpty(destPath) || !Path.Exists(destPath) || Transfer.SourceAccountPath == destPath)
        {
            return TransferCheckStatus.InvalidDestPath;
        }
        try
        {
            amount = decimal.Parse(amountString, NumberStyles.Currency);
        }
        catch
        {
            return TransferCheckStatus.InvalidAmount;
        }
        if (amount <= 0)
        {
            return TransferCheckStatus.InvalidAmount;
        }
        Transfer.DestinationAccountPath = destPath;
        Transfer.Amount = amount;
        return TransferCheckStatus.Valid;
    }
}