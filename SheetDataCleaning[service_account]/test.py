import gspread
import pandas as pd

from google.oauth2.service_account import Credentials
from sklearn.linear_model import LogisticRegression
from googleapiclient.errors import HttpError

SCOPES = ["https://www.googleapis.com/auth/spreadsheets"]

SPREADSHEET_ID = "yourID"

Credential = {
#Enter your credentials
}

class HysterisisController:
    def __init__(self, high:int, low:int, cooldown_period:int):
        self.high = high
        self.sensorV = high
        self.low = low
        self.cooldown_period = cooldown_period
        self.cooldown_reset = 5
        self.dState = False
        self.cooldown_timer = cooldown_period

    def update(self, sensor_value):
        if self.cooldown_timer > 0:
            self.sensorV = sensor_value if sensor_value != 0 else self.sensorV
            self.cooldown_timer -=1
            return
        if self.low > sensor_value and sensor_value != 0:
            self.sensorV = sensor_value
            self.dState = False
            self.cooldown_timer = self.cooldown_reset
            return
        if self.high < sensor_value:
            self.sensorV = sensor_value
            self.dState = True
            self.cooldown_timer = self.cooldown_reset
            return
        
def main():
    creds = Credentials.from_service_account_info(Credential, scopes = SCOPES)
    client = gspread.authorize(creds)

    try:
        workbook = client.open_by_key(SPREADSHEET_ID)
        sheet1 = workbook.get_worksheet(0)

        all_records = sheet1.get_all_records()
        df = pd.DataFrame({'A': all_records})

        lastRowIndex = df['A'].last_valid_index()
        print(f"lastRowIndex : {lastRowIndex}")
        Sensor_value = df.iloc[lastRowIndex].iloc[0]['Sensor Value']
        Current_cooldown_timer = df.iloc[lastRowIndex - 1].iloc[0]['Cooldown timer']
        Hy_cont = HysterisisController(90, 79, Current_cooldown_timer)
        Hy_cont.update(Sensor_value)

        df.drop(df.index[-1], inplace = True)

        df[['Sensor Value', 'Cooldown timer', 'dState']] = df['A'].apply(pd.Series)[['Sensor Value', 'Cooldown timer', 'Door State']]
        
        sensor_values = df['Sensor Value'].tolist()
        cooldown_timers = df['Cooldown timer'].tolist()
        target_values = df['dState'].tolist()
        

        X = pd.DataFrame({'Sensor Value' : sensor_values, 'Cooldown timers' : cooldown_timers})
        Y = target_values

        model = LogisticRegression(max_iter = 1000)
        model.fit(X, Y)

        predicted_state = model.predict([[Sensor_value, Current_cooldown_timer]])

        update_df = pd.DataFrame({
            'B' : ["edited"],
            'C' : [Hy_cont.cooldown_timer],
            'D' : [Hy_cont.sensorV],
            'E' : [Hy_cont.dState],
            'F' : [predicted_state]
        })

        data_to_write = update_df.astype(str).values.tolist()

        sheet1.update(data_to_write, f"B{lastRowIndex + 2}")

        print(update_df)
    except HttpError as error:
        print(error)


if __name__ == "__main__":
    main()