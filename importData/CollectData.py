import yfinance as yf
import pandas as pd

def download_stock_data(stock_symbol, period, interval):
    """
    Downloads stock data for the given stock symbol, period, and interval.
    Saves the data into an Excel file.
    
    :param stock_symbol: Symbol of the stock (e.g., 'AAPL')
    :param period: Time period for the data ('1y', '5y', etc.)
    :param interval: Data interval ('1h', '1d', '1wk', '1mo', '1y')
    """
    # Download stock data
    stock_data = yf.download(tickers=stock_symbol, period=period, interval=interval)
    # print(stock_data)
    # Save to Excel file
    excel_file = f"{stock_symbol}_{period}_{interval}.xlsx"
    stock_data.to_excel(excel_file)
    
    print(f"Data for {stock_symbol} saved to {excel_file}")

if __name__ == "__main__":
    stock_symbol = input("Enter the stock symbol (e.g., AAPL): ")
    
    period = input("Enter the period (e.g., '1y', '5y', 'max'): ")
    
    interval = input("Enter the interval (e.g., '1h' for hourly, '1d' for daily, '1wk' for weekly, '1mo' for monthly, '1y' for yearly): ")
    
    download_stock_data(stock_symbol, period, interval)
